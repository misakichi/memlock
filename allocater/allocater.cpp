#include "pch.h"

#include "allocater.h"
#include "nativealloc.h"

namespace memlock {
	void Allocator::setup(Diagnostics::Process^ proc)
	{
		nativeSetup(proc->Handle.ToPointer());
	}
	IntPtr Allocator::alloc(IntPtr ptr, IntPtr size)
	{
		auto ret = nativeAlloc(ptr.ToPointer(), size.ToInt64());
		auto retPtr = IntPtr(ret);
		return retPtr;
	}

	bool Allocator::free(IntPtr lpAddress, IntPtr dwSize)
	{
		auto ret = nativeFree(lpAddress.ToPointer(), dwSize.ToInt64());
		return ret;
	}
}