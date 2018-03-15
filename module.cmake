# ┌─┐┬ ┬┬─┐┌─┐┬─┐┌─┐  ┌─┐┬─┐┌─┐┌┬┐┌─┐┬ ┬┌─┐┬─┐┬┌─
# ├─┤│ │├┬┘│ │├┬┘├─┤  ├┤ ├┬┘├─┤│││├┤ ││││ │├┬┘├┴┐
# ┴ ┴└─┘┴└─└─┘┴└─┴ ┴  └  ┴└─┴ ┴┴ ┴└─┘└┴┘└─┘┴└─┴ ┴
# A Powerful General Purpose Framework
# More information in: https://aurora-fw.github.io/
#
# Copyright (C) 2017 Aurora Framework, All rights reserved.
#
# This file is part of the Aurora Framework. This framework is free
# software; you can redistribute it and/or modify it under the terms of
# the GNU Lesser General Public License version 3 as published by the
# Free Software Foundation and appearing in the file LICENSE included in
# the packaging of this file. Please review the following information to
# ensure the GNU Lesser General Public License version 3 requirements
# will be met: https://www.gnu.org/licenses/lgpl-3.0.html.

message(STATUS "Loading gengine-vulkan module...")

find_package(Vulkan REQUIRED)

if (NOT CONFIGURED_ONCE)
	set(AURORAFW_MODULE_GENGINE_VULKAN_SOURCE_DIR ${AURORAFW_MODULE_GENGINE_VULKAN_DIR}/src)
endif()

file(GLOB_RECURSE AURORAFW_MODULE_GENGINE_VULKAN_SOURCE ${AURORAFW_MODULE_GENGINE_VULKAN_SOURCE_DIR}/*.cpp)

add_library(aurorafw-gengine-vulkan SHARED ${AURORAFW_MODULE_GENGINE_VULKAN_SOURCE})

if(WIN32)
	target_link_libraries(aurorafw-gengine-vulkan ${VULKAN_STATIC_LIBRARY})
else()
target_link_libraries(aurorafw-gengine-vulkan ${VULKAN_LIBRARY})
endif()

include_directories(${AURORAFW_MODULE_GENGINE_VULKAN_DIR}/include)
