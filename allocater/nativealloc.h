#pragma once
#include <stdint.h>
#include <string>

struct GpuInfomation 
{
	std::wstring descName;
	size_t videoMemory;
};

namespace memlock {
	class MemlockNativeImpl;
	class NativeAlloc
	{
	public:
		static NativeAlloc* create();

		//cpu memory allocation
		virtual void* alloc(size_t dwSize) = 0;
		virtual bool free(void* lpAddress, size_t dwSize) = 0;

		//gpu memory allocation
		virtual int gpuNums() = 0;
		virtual GpuInfomation* getGpuInfo(uint32_t gpuNo) = 0;
		virtual void* gpuAlloc(uint32_t gpuNo, size_t dwSize) = 0;

		//initialize and finalize
		virtual bool initialize(void* hProc) = 0;
		virtual bool finalize() = 0;

	};

}