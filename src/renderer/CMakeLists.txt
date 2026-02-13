
set(GROUBIKS_VULKAN_RENDERER_SOURCES
    "vk_renderer.c"
    "vk_context.c"
    "vk_device_context.c"
    "vk_extras.c"
    "vk_fill_structs.c"
    "vk_graphics_pipeline.c"
    "vk_render_context.c"
    "vk_swapchain.c"
)
target_sources(groubiks
    PUBLIC ${GROUBIKS_VULKAN_RENDERER_SOURCES})