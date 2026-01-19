
#include <groubiks/renderer/vulkan_fill_structs.h>

void 
vk_fill_struct_swapchain_createinfo(VkSwapchainCreateInfoKHR* pCreateInfo,
    VkSurfaceKHR surface,
    VkSurfaceFormatKHR* pFormat,
    VkPresentModeKHR mode,
    VkExtent2D* pExtent,
    VkSurfaceCapabilitiesKHR* pCapabilities,
    u32* pQfis,
    u32  qfiCount)
{
    u32 imageCount;

    memzero(*pCreateInfo);
    imageCount = pCapabilities->minImageCount + 1;
    if (pCapabilities->maxImageCount != 0 &&
        (pCapabilities->minImageCount + 1) > pCapabilities->maxImageCount)
    { imageCount = pCapabilities->maxImageCount; }

    pCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    pCreateInfo->surface = surface;
    pCreateInfo->minImageCount = imageCount;
    pCreateInfo->imageFormat = pFormat->format;
    pCreateInfo->imageColorSpace = pFormat->colorSpace;
    pCreateInfo->imageExtent = *pExtent;
    pCreateInfo->imageArrayLayers = 1;
    pCreateInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    pCreateInfo->preTransform = pCapabilities->currentTransform;
    pCreateInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    pCreateInfo->presentMode = mode;
    pCreateInfo->clipped = VK_TRUE;


    if (qfiCount == 2 && pQfis[0] != pQfis[1]) {
        pCreateInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        pCreateInfo->queueFamilyIndexCount = 2;
        pCreateInfo->pQueueFamilyIndices = pQfis;
    }
    else {
        pCreateInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
}


void
vk_fill_struct_imageview_createinfo(VkImageViewCreateInfo* pCreateInfo,
    VkImage image,
    VkFormat format)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    pCreateInfo->image = image;
    pCreateInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
    pCreateInfo->format = format;
    pCreateInfo->components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    pCreateInfo->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    pCreateInfo->subresourceRange.baseMipLevel = 0;
    pCreateInfo->subresourceRange.levelCount = 1;
    pCreateInfo->subresourceRange.baseArrayLayer = 0;
    pCreateInfo->subresourceRange.layerCount = 1;
}


void
vk_fill_struct_device_createinfo(VkDeviceCreateInfo* pCreateInfo,
    const VkPhysicalDeviceFeatures* pFeatures,
    const VkDeviceQueueCreateInfo* pCreateInfos,
    u32 createInfoCount,
    const char* const* pLayers,
    u32 layerCount,
    const char* const* pExtensions,
    u32 extCount)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    pCreateInfo->pQueueCreateInfos = pCreateInfos;
    pCreateInfo->queueCreateInfoCount = createInfoCount;
    pCreateInfo->pEnabledFeatures = pFeatures;
    pCreateInfo->enabledLayerCount = layerCount;
    pCreateInfo->ppEnabledLayerNames = pLayers;
    pCreateInfo->enabledExtensionCount = extCount;
    pCreateInfo->ppEnabledExtensionNames = pExtensions;
}


void
vk_fill_struct_devicequeue_createinfo(VkDeviceQueueCreateInfo* pCreateInfo,
    u32 qFamilyIdx,
    const float* qPriority)
{
    memzero(*pCreateInfo);
    pCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    pCreateInfo->queueFamilyIndex = qFamilyIdx;
    pCreateInfo->queueCount = 1;
    pCreateInfo->pQueuePriorities = qPriority;
}