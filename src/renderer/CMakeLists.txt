
set(GROUBIKS_VULKAN_RENDERER_SOURCES
    "VulkanContext.c"
    "Extensions.c"
    "Devices.c"
    "ActiveDevice.c"
    "RenderContext.c"
    "GraphicsPipeline.c"
    "SwapChain.c"
    "Commands.c")

target_sources(groubiks
    PUBLIC ${GROUBIKS_VULKAN_RENDERER_SOURCES})