message("Loading gengine-vulkan module...")

find_package(Vulkan)

if(Vulkan_FOUND)
    add_definitions(-DAURORA_VULKAN_FOUND)
endif()

include_directories(${MODULE_GENGINE_VULKAN_DIR}/include)
