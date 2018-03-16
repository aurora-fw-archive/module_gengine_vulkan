/****************************************************************************
** ┌─┐┬ ┬┬─┐┌─┐┬─┐┌─┐  ┌─┐┬─┐┌─┐┌┬┐┌─┐┬ ┬┌─┐┬─┐┬┌─
** ├─┤│ │├┬┘│ │├┬┘├─┤  ├┤ ├┬┘├─┤│││├┤ ││││ │├┬┘├┴┐
** ┴ ┴└─┘┴└─└─┘┴└─┴ ┴  └  ┴└─┴ ┴┴ ┴└─┘└┴┘└─┘┴└─┴ ┴
** A Powerful General Purpose Framework
** More information in: https://aurora-fw.github.io/
**
** Copyright (C) 2017 Aurora Framework, All rights reserved.
**
** This file is part of the Aurora Framework. This framework is free
** software; you can redistribute it and/or modify it under the terms of
** the GNU Lesser General Public License version 3 as published by the
** Free Software Foundation and appearing in the file LICENSE included in
** the packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
****************************************************************************/

#include <AuroraFW/GEngine/Vulkan/Context.h>

#include <AuroraFW/STDL/STL/IOStream.h>

namespace AuroraFW {
	namespace GEngine {
		VKContext::VKContext(std::string &name)
			: _name(name)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		void VKContext::_init(GLFWwindow* window)
		{
#ifdef AFW__DEBUG
			uint32_t vk_layerCount;
			vk::enumerateInstanceLayerProperties(&vk_layerCount, AFW_NULLPTR);

			std::vector<vk::LayerProperties> vk_availableLayers(vk_layerCount);
			vk::enumerateInstanceLayerProperties(&vk_layerCount, vk_availableLayers.data());

			for (const char* vk_layerName : VKContext::validationLayers) {
				bool vk_layerFound = false;

				for (const vk::LayerProperties& vk_layerProperties : vk_availableLayers) {
					if (strcmp(vk_layerName, vk_layerProperties.layerName) == 0) {
						vk_layerFound = true;
						break;
					}
				}

				if (!vk_layerFound) {
					throw std::runtime_error("VulkanContext: validation layers requested, but not available!");
				}
			}
#endif
			vk::ApplicationInfo vk_appInfo(_name.c_str(), 0, "Aurora Framework", VK_MAKE_VERSION(AURORAFW_VERSION_MAJOR, AURORAFW_VERSION_MINOR, AURORAFW_VERSION_REVISION), VK_API_VERSION_1_0);

			vk::InstanceCreateInfo vk_createInfo(vk::InstanceCreateFlags(), &vk_appInfo);

			uint32_t glfwExtensionCount = 0;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef AFW__DEBUG
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

			vk_createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			vk_createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef AFW__DEBUG
			vk_createInfo.enabledLayerCount = static_cast<uint32_t>(VKContext::validationLayers.size());
			vk_createInfo.ppEnabledLayerNames = VKContext::validationLayers.data();
#else
			vk_createInfo.enabledLayerCount = 0;
#endif

			if(vk::createInstance(&vk_createInfo, AFW_NULLPTR, &_vkinstance) !=vk::Result::eSuccess)
				throw std::runtime_error("VulkanContext: failed to create Vulkan instance!");

#ifdef AFW__DEBUG
			VkDebugReportCallbackCreateInfoEXT vk_debug_createinfo = {};
			vk_debug_createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			vk_debug_createinfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			vk_debug_createinfo.pfnCallback = [](VkDebugReportFlagsEXT , VkDebugReportObjectTypeEXT , uint64_t , size_t , int32_t , const char* , const char* msg, void* )->VkBool32 {
				std::cerr << "validation layer: " << msg << std::endl;
				return VK_FALSE;
			};
			PFN_vkCreateDebugReportCallbackEXT func = (PFN_vkCreateDebugReportCallbackEXT) _vkinstance.getProcAddr("vkCreateDebugReportCallbackEXT");
			VkResult vk_ret;
			if (func != AFW_NULLPTR) {
				vk_ret = func(_vkinstance, &vk_debug_createinfo, AFW_NULLPTR, &_vkcallback);
			} else {
				vk_ret = VK_ERROR_EXTENSION_NOT_PRESENT;
			}
			if(vk_ret != VK_SUCCESS)
				throw std::runtime_error("failed to set up debug callback!");
#endif
			VkSurfaceKHR vk_surface;
			if (glfwCreateWindowSurface(_vkinstance, window, AFW_NULLPTR, &vk_surface) != VK_SUCCESS)
				throw std::runtime_error("VulkanContext: failed to create window surface!");
			_vksurface = vk_surface;

			uint32_t vk_deviceCount = AFW_NULLVAL;
			_vkinstance.enumeratePhysicalDevices(&vk_deviceCount, AFW_NULLPTR);

			if(vk_deviceCount == AFW_NULLVAL)
				throw std::runtime_error("VulkanContext: failed to find GPUs with Vulkan support!");
			std::vector<vk::PhysicalDevice> vk_devices(vk_deviceCount);
			_vkinstance.enumeratePhysicalDevices(&vk_deviceCount, vk_devices.data());

			for(const vk::PhysicalDevice& vk_pdevice : vk_devices) {
				uint32_t vk_queueFamilyCount = 0;
				vk_pdevice.getQueueFamilyProperties(&vk_queueFamilyCount, AFW_NULLPTR);

				std::vector<vk::QueueFamilyProperties> vk_queueFamilies(vk_queueFamilyCount);
				vk_pdevice.getQueueFamilyProperties(&vk_queueFamilyCount, vk_queueFamilies.data());

				Vulkan::QueueFamilyIndices vk_indices;
				int i = 0;
				for (const auto& vk_queueFamily : vk_queueFamilies) {
					if (vk_queueFamily.queueCount > 0 && vk_queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
						vk_indices.graphicsFamily = i;
					}

					VkBool32 vk_presentSupport = false;
					vk_pdevice.getSurfaceSupportKHR(i, _vksurface, &vk_presentSupport);

					if (vk_queueFamily.queueCount > 0 && vk_presentSupport) {
						vk_indices.presentFamily = i;
					}

					if (vk_indices.graphicsFamily >= 0 && vk_indices.presentFamily >= 0) {
						break;
					}

					i++;
				}

				if(vk_indices.graphicsFamily >= 0 && vk_indices.presentFamily >= 0) {
					_vkphysicalDevice = vk_pdevice;
					break;
				}
			}
		}

		void VKContext::_destroy()
		{
			_vkinstance.destroy(AFW_NULLPTR);
		}

		const std::vector<const char*> VKContext::validationLayers = {
				"VK_LAYER_LUNARG_standard_validation"
		};
	}
}
