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

#ifndef AURORAFW_GENGINE_VULKAN_CONTEXT_H
#define AURORAFW_GENGINE_VULKAN_CONTEXT_H

#include <AuroraFW/Global.h>
#if(AFW_TARGET_PRAGMA_ONCE_SUPPORT)
	#pragma once
#endif

#include <AuroraFW/Internal/Config.h>

#include <AuroraFW/GEngine/Vulkan/Global.h>

#include <AuroraFW/GEngine/API/Context.h>

namespace AuroraFW {
	namespace GEngine {
		namespace Vulkan {
			struct AFW_API QueueFamilyIndices {
				int graphicsFamily = -1;
				int presentFamily = -1;
			};
		}
		class AFW_API VKContext : public API::Context
		{
		public:
			VKContext(std::string& );

			inline static VKContext* getInstance() { return (VKContext*)_instance; }

#ifdef AFW__DEBUG
			static const std::vector<const char*> validationLayers;
#endif

		protected:
			void _init(GLFWwindow* ) override;
			void _destroy() override;

		private:
			std::string& _name;
			vk::Instance _vkinstance;
			vk::SurfaceKHR _vksurface;
			vk::PhysicalDevice _vkphysicalDevice;
			vk::Device _vkdevice;
			vk::Queue _vkgraphicsqueue;
			vk::Queue _vkpresentqueue;
#ifdef AFW__DEBUG
			VkDebugReportCallbackEXT _vkcallback;
#endif
		};
	}
}

#endif // AURORAFW_GENGINE_VULKAN_CONTEXT_H
