#pragma once

namespace memlock {
	class VulkanDeviceMemory {
	public:
		static VulkanDeviceMemory* create();
		virtual bool initialize() = 0;
	};
}