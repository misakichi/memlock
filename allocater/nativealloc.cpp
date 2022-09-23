#include "pch.h"
#include <Windows.h>
#include "nativealloc.h"

static HANDLE s_hProc;

void* nativeAlloc(void* lpAddress,size_t dwSize)
{
	SIZE_T minSz, maxSz;
	auto hProc = s_hProc;
	GetProcessWorkingSetSize(hProc, &minSz, &maxSz);
	minSz += dwSize;
	maxSz += dwSize;
	SetProcessWorkingSetSize(hProc, minSz, maxSz);

	auto base = VirtualAlloc(lpAddress, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	auto lock = VirtualLock(base, dwSize);
	auto er0 = GetLastError();
#if 0
	SYSTEM_INFO sysInfo = {};
	ULONG_PTR rqPages;
	GetSystemInfo(&sysInfo);
	rqPages = (dwSize + sysInfo.dwPageSize - 1) / sysInfo.dwPageSize;

	ULONG_PTR* pages = (ULONG_PTR*)VirtualAlloc(0,
		sizeof(ULONG_PTR)*rqPages,
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);;
	auto lock=VirtualLock(pages, sizeof(ULONG_PTR)*rqPages);
	auto er0 = GetLastError();

	auto physPages = AllocateUserPhysicalPages(hProc, &rqPages, pages);
	auto er1 = GetLastError();
	auto maped = MapUserPhysicalPages(base, rqPages, pages);
	auto er2 = GetLastError();
	base = VirtualAlloc(base, dwSize, MEM_COMMIT, PAGE_READWRITE);
	auto ret = VirtualLock(base, dwSize);
#endif
	return base;
}

bool nativeFree(void* lpAddress, size_t dwSize) 
{
	VirtualUnlock(lpAddress, dwSize);
	VirtualFree(lpAddress, 0, MEM_DECOMMIT);
	return VirtualFree(lpAddress, 0, MEM_RELEASE)!=0;
}
void nativeSetup(void* hProc)
{
	s_hProc = (HANDLE)hProc;
}