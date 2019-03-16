#ifndef DEVICE_CAPS_H_
#define DEVICE_CAPS_H_

#include <vector>

struct Caps
{
    // Physical device
    vk::PhysicalDeviceProperties deviceProperties;
    vk::PhysicalDeviceDriverPropertiesKHR driverProps;
    vk::PhysicalDeviceMemoryProperties memoryProps;

    // WSI Surface
    vk::SurfaceCapabilitiesKHR wsiSurfaceCaps;
    std::vector<vk::PresentModeKHR> wsiSurfacePresentModes;
    std::vector<vk::SurfaceFormatKHR> wsiSurfaceFormats;

    // Surface format
    vk::Format colorFormat;
    vk::Format depthFormat;
};

#endif // DEVICE_CAPS_H_
