
#include "allocater.h"
#include "nativealloc.h"

struct Context;

void cliDebugOut(const wchar_t* str)
{
	System::Diagnostics::Trace::TraceInformation(gcnew System::String(str));
}
namespace memlock {
	Allocator::Allocator()
	{
		m_allocator = nullptr;
	}
	bool Allocator::initilalize(Diagnostics::Process^ proc)
	{
		auto allocator = NativeAlloc::create();
		if (!allocator->initialize(proc->Handle.ToPointer())) {
			delete allocator;
			return false;
		}
		m_allocator = allocator;
		return true;
	}
	bool Allocator::finalize()
	{
		if (!m_allocator)
			return false;
		m_allocator->finalize();
		delete m_allocator;
		m_allocator = nullptr;
		return true;
	}

	IntPtr Allocator::alloc(IntPtr size)
	{
		if (!m_allocator)
			return IntPtr::Zero;
		auto ret = m_allocator->alloc(size.ToInt64());
		auto retPtr = IntPtr(ret);
		return retPtr;
	}

	bool Allocator::free(IntPtr ptr, IntPtr dwSize)
	{
		if (!m_allocator)
			return false;

		auto ret = m_allocator->free(ptr.ToPointer(), dwSize.ToInt64());
		return ret;
	}

	IntPtr Allocator::gpuAlloc(int gpuNum, IntPtr size)
	{
		if (!m_allocator)
			return IntPtr::Zero;

		auto ret = m_allocator->gpuAlloc(gpuNum, size.ToInt64());
		auto retPtr = IntPtr(ret);
		return retPtr;
	}

	
	System::Collections::Generic::List<GpuMemoryInfomation^>^ Allocator::getGpuInfomations()
	{
		if (!m_allocator)
			return nullptr;

		auto gpuNum = m_allocator->gpuNums();
		auto list = gcnew System::Collections::Generic::List<GpuMemoryInfomation^>();
		for (int gpu = 0; gpu < gpuNum; gpu++)
		{
			auto info = m_allocator->getGpuInfo(gpu);
			auto retInfo = gcnew GpuMemoryInfomation();
			retInfo->name = gcnew System::String(info->descName.c_str());
			retInfo->videoMemory = IntPtr((int64_t)info->videoMemory);
			list->Add(retInfo);
		}
		return list;
	}

}