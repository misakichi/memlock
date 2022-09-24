#pragma once
#include <stdint.h>

using namespace System;
namespace memlock {
	class NativeAlloc;

	public ref class GpuMemoryInfomation {
	public:
		System::String^ name;
		IntPtr videoMemory;
	};

	public ref class Allocator
	{
	public:
		Allocator();
		bool initilalize(Diagnostics::Process^ proc);
		bool finalize();

		IntPtr alloc(IntPtr size);
		bool free(IntPtr ptr, IntPtr dwSize);

		IntPtr gpuAlloc(int gpuIndex, IntPtr size);

		System::Collections::Generic::List<GpuMemoryInfomation^>^ getGpuInfomations();

	private:
		NativeAlloc* m_allocator;
	};
}
