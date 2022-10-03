#include <Windows.h>
#include "VulkanDeviceMemory.h"
#include "VulkanSDK\Include\vulkan\vulkan.h"
#include "VulkanSDK\Include\vulkan\vulkan_win32.h"

#include <stdio.h>
#include <stdarg.h>
#include <vector>
using namespace memlock;

extern void cliDebugOut(const wchar_t* str);

void log_printf(const wchar_t* format, ...)
{
	wchar_t buffer[4096];
	va_list args;
	va_start(args, format);
	vswprintf_s(buffer, format, args);
	va_end(args);

#ifdef _WIN32
	OutputDebugStringW(buffer);
#else
	printf_s(buffer);
#endif 
}

void log_printf_mb(const char* format, ...)
{
	char buffer[4096];
	wchar_t wbuffer[4096];
	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, format, args);
	va_end(args);

	mbstowcs_s(nullptr, wbuffer, buffer, 4096);

#ifdef _WIN32
	OutputDebugStringW(wbuffer);
#else
	printf_s(buffer);
#endif 
}
#define VK_VERSION(v)	VK_VERSION_MAJOR((v)), VK_VERSION_MINOR((v)), VK_VERSION_PATCH((v))
#define LOG_PRINTF(fmt,...)	log_printf(fmt,__VA_ARGS__)
#define LOG_PRINTF_MB(fmt,...)	log_printf_mb(fmt,__VA_ARGS__)
#define LOG_VULKAN_PRINTF(fmt,...)	LOG_PRINTF(L"[Vulkan] " fmt, __VA_ARGS__)
#define LOG_VULKAN_PRINTF_MB(fmt,...)	LOG_PRINTF_MB("[Vulkan] " fmt, __VA_ARGS__)
#define NM_LINE(nm)	nm ## __LINE__

#ifndef NDEBUG
VkResult vkCall(VkResult ret, const wchar_t* str)	{ 
	if (ret != VK_SUCCESS) {
		LOG_PRINTF(L"[Vulkan] %s", str);
		LOG_VULKAN_PRINTF(L"...Fail 0x%x\n", ret);
	}
	return ret;
}
#define VKCALL(x) vkCall(x,TEXT(#x))
#else
#define VKCALL(x) x
#endif


class VulkanDeviceMemoryImpl : public VulkanDeviceMemory {
public:
	bool initialize() override;

private:
	bool getExtensionsAndLayers();
	bool createInstance();

	std::vector<VkLayerProperties> m_layers;
	std::vector<VkExtensionProperties> m_extensions;
	std::vector<const char*> m_extensionsNames;
	std::vector<const char*> m_layerNames;

	VkInstance m_instance;
};

VulkanDeviceMemory* VulkanDeviceMemory::create()
{
	return new VulkanDeviceMemoryImpl();
}

bool VulkanDeviceMemoryImpl::initialize()
{
	if (!getExtensionsAndLayers())
		return false;
	if (!createInstance())
		return false;

	return true;

}
bool VulkanDeviceMemoryImpl::getExtensionsAndLayers()
{
	std::vector<const char*> extensionsNames;
	std::vector<const char*> layerNames;
	uint32_t tmpSize = 0;
	VkResult res;
	do {
		if (VKCALL(vkEnumerateInstanceLayerProperties(&tmpSize, NULL)))
			return false;

		if (tmpSize == 0) {
			m_layers.clear();
			break;
		}

		m_layers.resize(tmpSize);

		res = vkEnumerateInstanceLayerProperties(&tmpSize, m_layers.data());
	} while (res == VK_INCOMPLETE);

	//for (size_t i = 0; i < m_layers.size(); i++) {

	//	std::vector<VkExtensionProperties> layerExtensions;
	//	tmpSize = 0;
	//	do {
	//		if (VKCALL(vkEnumerateInstanceExtensionProperties(m_layers[i].layerName, &tmpSize, NULL)))
	//			return false;
	//		layerExtensions.resize(tmpSize);
	//		res = vkEnumerateInstanceExtensionProperties(m_layers[i].layerName, &tmpSize, layerExtensions.data());
	//	} while (res == VK_INCOMPLETE);
	//	m_extensions.insert(m_extensions.end(), layerExtensions.begin(), layerExtensions.end());
	//}

	for (size_t i = 0; i < m_layers.size(); i++)
	{
		bool use = strstr(m_layers[i].layerName, "varidation") != nullptr;
		LOG_VULKAN_PRINTF_MB("%s Layer %s(%d.%d.%d) : %s\n", use?"o":"x", m_layers[i].layerName, VK_VERSION(m_layers[i].specVersion), m_layers[i].description);
		layerNames.push_back(m_layers[i].layerName);
	}
	extensionsNames.resize(m_extensions.size());
	for (size_t i = 0; i < extensionsNames.size(); i++) 
	{
		LOG_VULKAN_PRINTF_MB("Extension %s(%d.%d.%d)\n", m_extensions[i].extensionName, VK_VERSION(m_extensions[i].specVersion));
		extensionsNames[i] = m_extensions[i].extensionName;
	}
//	extensionsNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
//#if defined(_WIN32)
//	extensionsNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
//#endif
#ifdef _DEBUG
	extensionsNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
	extensionsNames.swap(m_extensionsNames);
	layerNames.swap(m_layerNames);

	return true;
}

bool VulkanDeviceMemoryImpl::createInstance()
{
	uint32_t apiVersion = VK_API_VERSION_1_0;
	if (VK_SUCCESS == vkEnumerateInstanceVersion(&apiVersion)) {
		auto vmajor = VK_VERSION_MAJOR(apiVersion);
		auto vminor = VK_VERSION_MINOR(apiVersion);
		auto vpatch = VK_VERSION_PATCH(apiVersion);
		wchar_t buffer[256];
		swprintf_s(buffer, L"Got Api Version : %d %d %d\n", vmajor, vminor, vpatch);
		LOG_VULKAN_PRINTF(L"Got Api Version : %d %d %d\n", vmajor, vminor, vpatch);

	}


	VkApplicationInfo ai = {};
	ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ai.pNext = NULL;
	ai.pApplicationName = "memlock";
	ai.applicationVersion = 1;
	ai.pEngineName = "memlock";
	ai.engineVersion = 1;
	ai.apiVersion = apiVersion;


	VkInstanceCreateInfo ici = {};
	ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	ici.pApplicationInfo = &ai;
	ici.ppEnabledExtensionNames = m_extensionsNames.data();
	ici.enabledExtensionCount = m_extensionsNames.size();
	ici.enabledLayerCount = m_layerNames.size();
	if (!m_layerNames.empty()) {
		ici.ppEnabledLayerNames = m_layerNames.data();
	}
	if (VKCALL(vkCreateInstance(&ici, nullptr, &m_instance)) != VK_SUCCESS) {
		return false;
	}
	LOG_VULKAN_PRINTF("Create Instance OK\n");
	return true;

}