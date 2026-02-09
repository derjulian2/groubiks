
set(GROUBIKS_VULKAN_RENDERER_SOURCES
    "vulkan_context.c"
    "vulkan_device_context.c"
    "vulkan_extras.c"
    "vulkan_fill_structs.c"
    "vulkan_graphics_pipeline.c"
    "vulkan_render_context.c"
    "vulkan_swapchain.c"
)
target_sources(groubiks
    PUBLIC ${GROUBIKS_VULKAN_RENDERER_SOURCES})