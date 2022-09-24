#include <Windows.h>
#include "nativealloc.h"

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi.h>
#include <vector>
#include <array>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;
namespace memlock {

	struct D3dDevice {
		~D3dDevice()
		{
			adapter = nullptr;
			d3dCtx = nullptr;
			d3dDevice = nullptr;
		}
		ComPtr<IDXGIAdapter> adapter;
		ComPtr<ID3D11Device> d3dDevice;
		ComPtr<ID3D11DeviceContext> d3dCtx;
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
		}
		return nullptr;
	}

	bool NativeAllocImpl::initialize(void* hProc)
	{
		procId = GetProcessId((HANDLE)hProc);

		ComPtr<IDXGIFactory> factory;
		CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)factory.GetAddressOf());
		if (factory == nullptr) {
			return false;
		}
		std::array<D3D_FEATURE_LEVEL, 2> featureLevels =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		D3D_FEATURE_LEVEL featureLevel;
		for (int i = 0;; i++) {
			ComPtr<IDXGIAdapter> adapter;
			factory->EnumAdapters(i, adapter.GetAddressOf());
			if (adapter == nullptr)
				break;

			D3dDevice device;
#ifdef _DEBUG
			auto ret = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels.data(), featureLevels.size(), D3D11_SDK_VERSION, device.d3dDevice.GetAddressOf(), &featureLevel, device.d3dCtx.GetAddressOf());
			if (FAILED(ret))
#endif
			{
				ret = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, featureLevels.data(), featureLevels.size(), D3D11_SDK_VERSION, device.d3dDevice.GetAddressOf(), &featureLevel, device.d3dCtx.GetAddressOf());
			}
			if (SUCCEEDED(ret)) {
				DXGI_ADAPTER_DESC desc;
				device.adapter = adapter;
				device.adapter->GetDesc(&desc);
				device.info.videoMemory = desc.DedicatedVideoMemory;
				device.info.descName = desc.Description;

				d3dDevices.push_back(device);
			}
		}
		return true;
	}

	bool NativeAllocImpl::finalize()
	{
		d3dDevices.clear();		
		return true;
	}

}