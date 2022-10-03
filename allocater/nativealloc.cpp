#include <Windows.h>
#include "nativealloc.h"

#include <vector>
#include <array>
#include <psapi.h>


#define USE_D3D_VERSION	11
#if defined(USE_D3D_VERSION) && USE_D3D_VERSION==11
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#elif defined(USE_D3D_VERSION)==12
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#else
#define USE_VUKKAN
#include "VulkanSDK\Include\vulkan\vulkan.h"
#include "VulkanDeviceMemory.h"
#endif

#ifdef USE_D3D_VERSION
#include <wrl/client.h>
#include <dxgi1_4.h>
#pragma comment(lib, "dxgi.lib")
using Microsoft::WRL::ComPtr;
#define QUERYINTERFACE_COMPTR(ptr)	__uuidof(decltype(ptr)::InterfaceType), (void**)ptr.GetAddressOf()
#endif

namespace memlock {

	struct D3dDevice {
		~D3dDevice()
		{
#ifdef USE_D3D_VERSION
			adapter = nullptr;
#if USE_D3D_VERSION==11
			d3dCtx = nullptr;
#endif
			d3dDevice = nullptr;
#endif
		}
#ifdef USE_D3D_VERSION
		ComPtr<IDXGIAdapter> adapter;
#if USE_D3D_VERSION==11
		ComPtr<ID3D11Device> d3dDevice;
		ComPtr<ID3D11DeviceContext> d3dCtx;
#elif USE_D3D_VERSION==12
		ComPtr<ID3D12Device> d3dDevice;
		ComPtr<ID3D12Heap1> d3dHeap;
#endif
#else

#endif
		GpuInfomation info;
	};

	class NativeAllocImpl : public NativeAlloc
	{
	public:

		//cpu memory allocation
		virtual void* alloc(size_t dwSize) override;
		virtual bool free(void* lpAddress, size_t dwSize) override;

		//gpu memory allocation
		virtual int gpuNums() override;
		virtual GpuInfomation* getGpuInfo(uint32_t gpuNo) override;
		virtual void* gpuAlloc(uint32_t gpuNo, size_t dwSize) override;

		//initialize and finalize
		virtual bool initialize(void* hProc) override;
		virtual bool finalize() override;

	private:
		DWORD procId;
		std::vector<D3dDevice> d3dDevices;
#ifdef USE_VUKKAN
		VulkanDeviceMemory* vulkanDevice;
#endif
	};



	NativeAlloc* NativeAlloc::create()
	{
		return new NativeAllocImpl();
	}

	void* NativeAllocImpl::alloc(size_t dwSize)
	{
		void* base = nullptr;
		constexpr int RETRY_MAX = 2;
		int retry = 0;
		auto hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_QUOTA, FALSE, procId);
		if (!hProc)
			return nullptr;

		SIZE_T minSz = 0;
		SIZE_T maxSz = 0;
		GetProcessWorkingSetSize(hProc, &minSz, &maxSz);
		minSz += dwSize;
		maxSz += dwSize;
		SetProcessWorkingSetSize(hProc, minSz, maxSz);

		bool success = false;
		base = VirtualAlloc(0, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (base) {
			auto lock = VirtualLock(base, dwSize);
			if (!lock) {
				auto err = GetLastError();
				VirtualFree(base, 0, MEM_DECOMMIT);
				VirtualFree(base, 0, MEM_RELEASE);
				base = nullptr;
				SetLastError(err);
			}
		}

		CloseHandle(hProc);
		return base;
	}

	bool NativeAllocImpl::free(void* lpAddress, size_t dwSize)
	{
		VirtualUnlock(lpAddress, dwSize);
		VirtualFree(lpAddress, 0, MEM_DECOMMIT);
		return VirtualFree(lpAddress, 0, MEM_RELEASE) != 0;
	}

	int NativeAllocImpl::gpuNums()
	{
		return (int)d3dDevices.size();
	}


	GpuInfomation* NativeAllocImpl::getGpuInfo(uint32_t gpuNo)
	{
		if (d3dDevices.size() > gpuNo)
		{
			auto& dev = d3dDevices[gpuNo];
			return &dev.info;
		}
		return nullptr;

	}

	void* NativeAllocImpl::gpuAlloc(uint32_t gpuNo, size_t dwSize)
	{
		if (d3dDevices.size() > gpuNo)
		{
			auto& dev = d3dDevices[gpuNo];
#if USE_D3D_VERSION==11
			D3D11_BUFFER_DESC desc = {};
			desc.ByteWidth = dwSize;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			ID3D11Buffer* buffer = nullptr;
			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = malloc(dwSize);
			dev.d3dDevice->CreateBuffer(&desc, &data, &buffer);
			::free((void*)data.pSysMem);
			return buffer;
#elif USE_D3D_VERSION==12
			dwSize = (dwSize + 0xff) & ~0xff;
			ID3D12Resource* buffer;
			ComPtr<ID3D12Heap> heap;
			D3D12_RESOURCE_DESC desc = { 
				D3D12_RESOURCE_DIMENSION_BUFFER, 0, 
				(UINT64)dwSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, 
				{ 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE
			};
			auto prop = dev.d3dDevice->GetCustomHeapProperties(1<<0, D3D12_HEAP_TYPE_DEFAULT);
			auto allocInfo = dev.d3dDevice->GetResourceAllocationInfo(1 << 0, 1, &desc);
			//CD3DX12_HEAP_DESC heapDesc(info, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);

			D3D12_HEAP_DESC heapDesc = {};
			heapDesc.SizeInBytes = allocInfo.SizeInBytes;
			heapDesc.Alignment = allocInfo.Alignment;
			heapDesc.Properties= prop;
			heapDesc.Flags = D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;
			auto heapRet = dev.d3dDevice->CreateHeap(&heapDesc, QUERYINTERFACE_COMPTR(heap));
			dev.d3dDevice->CreatePlacedResource(heap.Get(), 0, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(*buffer), (void**)&buffer);
			
			D3D12_HEAP_PROPERTIES heapProp;
			D3D12_HEAP_FLAGS heapFlag;
			D3D12_GPU_VIRTUAL_ADDRESS addr = {};
			if (buffer)
			{
				buffer->GetHeapProperties(&heapProp, &heapFlag);
				addr = buffer->GetGPUVirtualAddress();

				auto hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procId);
				PSAPI_WORKING_SET_EX_INFORMATION   vaWs = {};
				vaWs.VirtualAddress = (void*)addr;
				QueryWorkingSetEx(hProc, &vaWs, sizeof(PSAPI_WORKING_SET_INFORMATION));
				//auto wsInfoSize = sizeof(PSAPI_WORKING_SET_INFORMATION) + sizeof(PSAPI_WORKING_SET_BLOCK) * (tmpWSInfo.NumberOfEntries + 99);
				//PSAPI_WORKING_SET_INFORMATION* wsInfos = (PSAPI_WORKING_SET_INFORMATION*)malloc(wsInfoSize);
				//QueryWorkingSet(hProc, wsInfos, sizeof(PSAPI_WORKING_SET_INFORMATION));
				//for (size_t i = 0; i < wsInfos->NumberOfEntries; i++)
				//{
				//	auto ws =wsInfos->WorkingSetInfo[i];
				//	if (ws.VirtualPage == addr) {
				//		buffer->Release();
				//		heap = nullptr;
				//		buffer = nullptr;
				//	}
				//}

				CloseHandle(hProc);
			}
			return buffer;

#endif
		}
		return nullptr;
	}

	bool NativeAllocImpl::initialize(void* hProc)
	{
		procId = GetProcessId((HANDLE)hProc);

#ifndef USE_D3D_VERSION
		vulkanDevice = VulkanDeviceMemory::create();
		return vulkanDevice->initialize();
#else
		ComPtr<IDXGIFactory4> factory;
		CreateDXGIFactory1(QUERYINTERFACE_COMPTR(factory));
		if (factory == nullptr) {
			return false;
		}
		std::array<D3D_FEATURE_LEVEL, 2> featureLevels =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
#if USE_D3D_VERSION==12
#if defined(_DEBUG)
		ComPtr<ID3D12Debug>	debugController;
		if(SUCCEEDED(D3D12GetDebugInterface(QUERYINTERFACE_COMPTR(debugController)))) {
			debugController->EnableDebugLayer();
		}
#endif
#endif

		D3D_FEATURE_LEVEL featureLevel;
		for (int i = 0;; i++) {
			ComPtr<IDXGIAdapter1> adapter;
			factory->EnumAdapters1(i, adapter.GetAddressOf());
			if (adapter == nullptr)
				break;
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			D3dDevice device;
			HRESULT ret = -1;
#if USE_D3D_VERSION==11

#ifdef _DEBUG
			ret = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels.data(), featureLevels.size(), D3D11_SDK_VERSION, device.d3dDevice.GetAddressOf(), &featureLevel, device.d3dCtx.GetAddressOf());
			if (FAILED(ret))
#endif
			{
				ret = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, featureLevels.data(), featureLevels.size(), D3D11_SDK_VERSION, device.d3dDevice.GetAddressOf(), &featureLevel, device.d3dCtx.GetAddressOf());
			}
			if (FAILED(ret)) {
				continue;
			}
			device.info.videoMemory = desc.DedicatedVideoMemory;
			device.info.descName = desc.Description;
			d3dDevices.push_back(device);
#elif USE_D3D_VERSION==12
			ret = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, QUERYINTERFACE_COMPTR(device.d3dDevice));
			if (FAILED(ret)) {
				continue;
			}
			auto nodes = device.d3dDevice->GetNodeCount();
			for (int node = 0; node < nodes; node++)
			{
				if (desc.DedicatedVideoMemory > 0)
				{
					auto prop = device.d3dDevice->GetCustomHeapProperties(1 << 0, D3D12_HEAP_TYPE_DEFAULT);
					device.d3dDevice->CreateCommittedResource(&prop,D3D12_HEAP_FLAG_CREATE_NOT_ZEROED,)
				}
			}
			device.info.videoMemory = desc.DedicatedVideoMemory;
			device.info.descName = desc.Description;
			d3dDevices.push_back(device);		
#endif

		}
#endif
		return true;
	}

	bool NativeAllocImpl::finalize()
	{
		d3dDevices.clear();		
		return true;
	}

}