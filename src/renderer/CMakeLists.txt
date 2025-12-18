
set(GROUBIKS_VULKAN_RENDERER_SOURCES
    "VulkanContext.c"
    "Extensions.c"
    "Devices.c"
    "ActiveDevice.c")

target_sources(groubiks
    PUBLIC ${GROUBIKS_VULKAN_RENDERER_SOURCES})