
set(GROUBIKS_VULKAN_RENDERER_SOURCES
    # "vulkan_context.c"
    "vulkan_extras.c"
    "vulkan_device_context.c"
    # "RenderContext.c"
    # "GraphicsPipeline.c"
    # "vulkan_swapchain.c"
    # "Commands.c")
)
target_sources(groubiks
    PUBLIC ${GROUBIKS_VULKAN_RENDERER_SOURCES})