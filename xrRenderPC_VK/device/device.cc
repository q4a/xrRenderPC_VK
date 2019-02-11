#include <algorithm>
#include <vector>

#include "common.h"
#include "device/caps.h"
#include "device/device.h"

Hw hw;

// TODO: This should be defined application wide
constexpr pcstr engineName = "OpenXRay";
constexpr pcstr appName = "S.T.A.L.K.E.R.: Call Of Pripyat";

/**
 * Layers
 */
std::vector<pcstr> instanceLayers =
{
#if DEBUG
    "VK_LAYER_LUNARG_standard_validation",
    "VK_LAYER_LUNARG_parameter_validation",
    "VK_LAYER_LUNARG_core_validation"
#endif
};

/**
 *Instance extensions
 */
std::vector<pcstr> instanceExtensions =
{
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(WIN32)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
};

/** 
 * Device extensions
 */
std::vector<pcstr> deviceExtensions =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/** 
 * Amount of command buffers dedicated to data transfer
 */
constexpr std::size_t ctrl_buffers_num = 1;

/**
 *
 */
void
Hw::CreateInstance()
{
    const auto appInfo = vk::ApplicationInfo()
        .setApiVersion(VK_API_VERSION_1_1)
        .setPApplicationName(appName)
        .setEngineVersion(Core.GetBuildId())
        .setPEngineName(engineName);

    const auto instanceCreateInfo = vk::InstanceCreateInfo()
        .setPApplicationInfo(&appInfo)
        .setEnabledLayerCount(instanceLayers.size())
        .setPpEnabledLayerNames(instanceLayers.data())
        .setEnabledExtensionCount(instanceExtensions.size())
        .setPpEnabledExtensionNames(instanceExtensions.data());

    instance_ = vk::createInstanceUnique(instanceCreateInfo);
}


/**
 *
 */
void
Hw::SelectGpu()
{
    /* Check for GPU availability */
    const auto physicalDevices = instance_->enumeratePhysicalDevices();

    for (const auto& device : physicalDevices)
    {
        const auto queuesProperties = device.getQueueFamilyProperties();
        auto predicate =
            [](const auto& props)
            {
                return !!(props.queueFlags & vk::QueueFlagBits::eGraphics);
            };
        const auto queueIdx =
            std::find_if( queuesProperties.cbegin()
                        , queuesProperties.cend()
                        , predicate
            );

        graphicsQfamilyIdx =
            static_cast<std::uint32_t>(std::distance( queuesProperties.cbegin()
                                                    , queueIdx
            ));

        // TODO: Here we finish enumeration by grabbing first graphics capable
        // device. Need to consider other cases (probably exotic ones) when the
        // system has several GPUs but only one of them are compatible with WS.
        gpu_ = device;
        break;
    }
    R_ASSERT2( graphicsQfamilyIdx != invalidQindex
             , "There is no GPU in the system"
    );

    // Print some fancy information like R1 render did
    caps.deviceProperties = gpu_.getProperties();
    Msg( "* GPU [vendor:%X]-[device:%X]: %s"
       , caps.deviceProperties.vendorID
       , caps.deviceProperties.deviceID
       , caps.deviceProperties.deviceName
    );

    // TODO: driver properties?

    caps.memoryProps = gpu_.getMemoryProperties();
}


/**
 *
 */
void
Hw::CreateLogicalDevice()
{
    // TODO: by now assume that both presentation and graphics queues
    // are from one family
    R_ASSERT(graphicsQfamilyIdx == presentQfamilyIdx);

    const float graphicsQpriority = 0.0f;
    const auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo()
        .setQueueCount(1)
        .setPQueuePriorities(&graphicsQpriority)
        .setQueueFamilyIndex(graphicsQfamilyIdx);

    const auto deviceCreateInfo = vk::DeviceCreateInfo()
        .setPQueueCreateInfos(&deviceQueueCreateInfo)
        .setQueueCreateInfoCount(1)
        .setEnabledExtensionCount(deviceExtensions.size())
        .setPpEnabledExtensionNames(deviceExtensions.data());

    device = gpu_.createDeviceUnique(deviceCreateInfo);

    submission_q = device->getQueue(graphicsQfamilyIdx, 0);
}


/**
 *
 */
void
Hw::CreateCommandBuffers()
{
    const auto cmd_pool_create_info = vk::CommandPoolCreateInfo()
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(graphicsQfamilyIdx); // TODO: separate transfer queue
    cmd_pool_ = device->createCommandPoolUnique(cmd_pool_create_info);

    const auto cmd_buffer_alloc_info = vk::CommandBufferAllocateInfo()
        .setCommandBufferCount(ctrl_buffers_num)
        .setCommandPool(cmd_pool_.get())
        .setLevel(vk::CommandBufferLevel::ePrimary);
    ctrl_cmd_buf_ = device->allocateCommandBuffersUnique(cmd_buffer_alloc_info);
}


/**
 *
 */
void
Hw::CreateDevice
        ( SDL_Window * const sdlWnd
        )
{
    CreateInstance();

    SelectGpu();

    CreateSurface(sdlWnd);

    CreateLogicalDevice();

    CreateSwapchain();

    CreateCommandBuffers();
}


/**
 *
 */
void
Hw::DestroyCommandBuffers()
{
    for (auto& buffer : ctrl_cmd_buf_)
    {
        buffer.reset();
    }

    cmd_pool_.reset();
}


/**
 *
 */
void
Hw::DestroyDevice()
{
    DestroyCommandBuffers();

    DestroySwapchain();

    device.reset();
    instance_.reset();
}


/**
 *
 */
void
Hw::ResetDevice()
{
    /*
     * Likely this function called to re-create the swapchain
     */

    device->waitIdle();

    DestroySwapchain();
    CreateSwapchain();
}


void
Hw::OnAppActivate()
{
    // TODO: really needed?
}


void
Hw::OnAppDeactivate()
{
    // TODO: really needed?
}
