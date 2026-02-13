
#ifndef GROUBIKS_COMPILETIME_CONFIG_H
#define GROUBIKS_COMPILETIME_CONFIG_H

/**
 * @file compile_config.h
 * @author Julian Benzel
 * @brief compile-time options for groubiks bundled in one file.
 */

#define RENDERER_LOG 4

#define DEFAULT_WIN_WIDTH 640
#define DEFAULT_WIN_HEIGHT 480
#define DEFAULT_APPLICATION_NAME "groubiks - rubiks-cube simulator"

#define VK_VALIDATIONLAYERS (const char*[]) { "VK_LAYER_KHRONOS_validation" }
#define VK_EXTENSIONS (const char*[]) { VK_EXT_DEBUG_UTILS_EXTENSION_NAME }
#define VK_DEVICE_EXTENSIONS (const char*[]) { VK_KHR_SWAPCHAIN_EXTENSION_NAME }

#define VK_NUM_VALIDATIONLAYERS 1
#define VK_NUM_EXTENSIONS 1
#define VK_NUM_DEVICE_EXTENSIONS 1

#define GROUBIKS_VERTEX_SHADER_SOURCE_PATH   "/home/julian/Projects/groubiks/src/shaders/default.vert"
#define GROUBIKS_FRAGMENT_SHADER_SOURCE_PATH "/home/julian/Projects/groubiks/src/shaders/default.frag"

/**
 * --- success-log-strings ---
 */
#define VK_CONTEXT_CREATE_SUCCESS_STR "created vulkan-context"
#define VK_CONTEXT_DEVICE_SETUP_SUCCESS_STR "retrieved physical devices"
#define VK_CONTEXT_INSTANCE_SETUP_SUCCESS_STR "setup vulkan-instance"
#define VK_CONTEXT_DEBUG_MESSENGER_SETUP_SUCCESS_STR "setup vulkan-debug-messenger"

#define VK_PIPELINE_CREATE_SUCCESS_STR "created vulkan-graphics-pipeline"
#define VK_PIPELINE_RENDERPASS_SETUP_SUCCESS_STR "setup renderpass"
#define VK_SHADERMOD_SETUP_SUCCESS_STR "setup shadermodule"

#define VK_SWAPCHAIN_CREATE_SUCCESS_STR "create vulkan-swapchain"
#define VK_SWAPCHAIN_IMAGE_HANDLES_SUCCESS_FSTR "retrieved %d image-handles"
#define VK_SWAPCHAIN_SURFACE_FMTS_SUCCESS_STR "retrieved surface-formats"
#define VK_SWAPCHAIN_PRESENTMODES_SUCCESS_STR "retrieved presentmodes"
#define VK_SWAPCHAIN_SURFACE_CAPS_SUCCESS_STR "retrieved surface-capabilities"
#define VK_SWAPCAHIN_IMAGEVIEW_SUCCESS_FSTR "setup %d imageviews"

#define VK_RENDER_CONTEXT_CREATE_SUCCESS_STR "setup render-context"
#define VK_RENDER_CONTEXT_FRAMEBUF_SUCCESS_STR "setup framebuffers"
#define VK_CMD_CONTEXT_CREATE_SUCCESS_STR "setup command-context"

#define VK_RENDERER_CREATE_SUCCESS_STR "created vulkan-renderer"

/**
 * --- error-log-strings ---
 */
#define VK_CONTEXT_CREATE_FAIL_STR "failed to create vulkan-context"
#define VK_CONTEXT_DEVICE_SETUP_FAIL_STR "failed to retrieve available physical devices"
#define VK_CONTEXT_INSTANCE_SETUP_FAIL_STR "failed to setup vulkan-instance"
#define VK_CONTEXT_DEBUG_MESSENGER_SETUP_FAIL_STR "failed to setup vulkan-debug-messenger"
#define VK_CONTEXT_DEBUG_MESSENGER_CLEANUP_FAIL_STR "failed to cleanup vulkan-debug-messenger. something wen't wrong badly"

#define VK_PIPELINE_CREATE_FAIL_STR "failed to create vulkan-graphics-pipeline"
#define VK_PIPELINE_RENDERPASS_SETUP_FAIL_STR "failed to setup renderpass"
#define VK_SHADERMOD_SETUP_FAIL_STR "failed to setup shadermodule"

#define VK_SWAPCHAIN_CREATE_FAIL_STR "failed to create vulkan-swapchain"
#define VK_SWAPCHAIN_IMAGE_HANDLES_FAIL_STR "failed to retrieve image-handles"
#define VK_SWAPCHAIN_SURFACE_FMTS_FAIL_STR "failed to retrieve surface-formats"
#define VK_SWAPCHAIN_PRESENTMODES_FAIL_STR "failed to retrieve presentmodes"
#define VK_SWAPCHAIN_SURFACE_CAPS_FAIL_STR "failed to retrieve surface-capabilites"
#define VK_SWAPCHAIN_IMAGEVIEW_FAIL_STR "failed to setup imageviews"

#define VK_RENDER_CONTEXT_CREATE_FAIL_STR "failed to create render-context"
#define VK_RENDER_CONTEXT_FRAMEBUF_FAIL_STR "failed to setup framebuffers"
#define VK_RENDER_CONTEXT_RECORD_FAIL_STR "failed to record commandbuffer"
#define VK_RENDER_CONTEXT_DRAW_FAIL_STR "failed to execute drawing-command"
#define VK_CMD_CONTEXT_CREATE_FAIL_STR "failed to create command-context"

#define VK_RENDERER_CREATE_FAIL_STR "failed to create vulkan-renderer"

#endif