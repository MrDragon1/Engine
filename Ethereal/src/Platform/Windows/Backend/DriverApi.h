#pragma once
namespace Ethereal {
namespace Backend {
/*
 * DriverApi aggregates the hardware operations that support by OpenGL & Vulkan. Only this class can
 * access the operation per platform. DriverApi abstract the platform and provide services for high
 * level layers.
 * */
class DriverApi {

};

} // Ethereal
} // Backend
