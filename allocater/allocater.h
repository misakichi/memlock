#pragma once
#include <stdint.h>

using namespace System;
namespace memlock {
	public ref class Allocator
	{
	public:
		static IntPtr alloc(IntPtr ptr, IntPtr size);
		static bool free(IntPtr lpAddress, IntPtr dwSize);
		static void setup(Diagnostics::Process^ proc);
	};
}
