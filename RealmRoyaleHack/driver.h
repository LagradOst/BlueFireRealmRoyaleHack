#pragma once
#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <ntstatus.h>
#include <atomic>
#include <mutex>
#include <TlHelp32.h>
#include <immintrin.h>
#include <cstdint>
#include <cassert>
#include "utils.h"

#pragma comment(lib, "ntdll.lib")

typedef struct _TslEntity
{
	uint64_t pObjPointer;
	int ID;

	uint64_t Mesh;
	uint64_t PlayerController;
	uint64_t DamageController;
	uint64_t PlayerState;

}TslEntity;

typedef enum _MEMORY_INFORMATION_CLASS {
	MemoryBasicInformation
} MEMORY_INFORMATION_CLASS;

namespace driver
{

	typedef enum _REQUEST_TYPE : UINT {
		WRITE,
		READ,
		PROTECT,
		ALLOC,
		FREE,
		MODULE,
		MAINBASE,
		PROCESSINFO,
		THREADCALL
	} REQUEST_TYPE;

	typedef struct _REQUEST_DATA {
		ULONG64* MaggicCode;
		UINT Type;
		PVOID Arguments;
		NTSTATUS* Status;
	} REQUEST_DATA, * PREQUEST_DATA;

	typedef struct _REQUEST_WRITE {
		DWORD ProcessId;
		PVOID Dest;
		PVOID Src;
		DWORD Size;
		BOOL bPhysicalMem;
	} REQUEST_WRITE, * PREQUEST_WRITE;

	typedef struct _REQUEST_READ {
		DWORD ProcessId;
		PVOID Dest;
		PVOID Src;
		DWORD Size;
		BOOL bPhysicalMem;
	} REQUEST_READ, * PREQUEST_READ;

	typedef struct _REQUEST_PROTECT {
		DWORD ProcessId;
		PVOID Address;
		DWORD Size;
		PDWORD InOutProtect;
	} REQUEST_PROTECT, * PREQUEST_PROTECT;

	typedef struct _REQUEST_ALLOC {
		DWORD ProcessId;
		PVOID OutAddress;
		DWORD Size;
		DWORD Protect;
	} REQUEST_ALLOC, * PREQUEST_ALLOC;

	typedef struct _REQUEST_FREE {
		DWORD ProcessId;
		PVOID Address;
	} REQUEST_FREE, * PREQUEST_FREE;

	typedef struct _REQUEST_MODULE {
		DWORD ProcessId;
		WCHAR Module[0xFF];
		PBYTE* OutAddress;
		DWORD* OutSize;
	} REQUEST_MODULE, * PREQUEST_MODULE;

	typedef struct _REQUEST_MAINBASE {
		DWORD ProcessId;
		PBYTE* OutAddress;
	} REQUEST_MAINBASE, * PREQUEST_MAINBASE;

	typedef struct _REQUEST_PROCESSINFO {
		DWORD ProcessId;
		PVOID BaseAddress;
		MEMORY_INFORMATION_CLASS MemoryInformationClass;
		PVOID MemoryInformation;
		SIZE_T MemoryInformationLength;
		PSIZE_T ReturnLength;
	} REQUEST_PROCESSINFO, * PREQUEST_PROCESSINFO;

	typedef struct Module { uint64_t addr; DWORD size; };

	namespace detail
	{

		inline PVOID SharedBuffer;
		inline HANDLE hDriver;
		inline ULONG64 MAGGICCODE = 0x59002360218c1e2dul;
		inline BOOL bPhysicalMode = FALSE;
		inline UINT ProcessId;
		inline UINT ThreadId;
	}

	bool Init(BOOL PhysicalMode);
	bool Attach(const char* Processname, wchar_t* Classname = 0);

	namespace driver_impl {

		NTSTATUS SendRequest(const UINT type, const PVOID args);
		UINT GetProcessId(const char* process_name);
		driver::Module GetModuleBase(const wchar_t* ModuleName);
		bool ReadProcessMemory(PVOID src, PVOID dest, DWORD size);
		bool WriteProcessMemory(const uintptr_t address, const uintptr_t buffer, const size_t size);
		NTSTATUS Protect(PVOID address, DWORD size, PDWORD inOutProtect);
		NTSTATUS ProcessInfo(PVOID baseAddress, MEMORY_INFORMATION_CLASS memoryInformationClass, PVOID memoryInformation, SIZE_T memoryInformationLength, PSIZE_T returnLength);

		template <typename T>
		T read(const uintptr_t address)
		{
			T buffer{ };
			if(address > 0 && address != 0xCCCCCCCCCCCCCCCC) {
				ReadProcessMemory((PVOID)address, (PVOID)&buffer, sizeof(T));
			}
			return buffer;
		}

		template<typename T>
		inline bool read(uint64_t address, T& value) {
			return ReadProcessMemory((PVOID)address, (PVOID)&value, sizeof(T));
		}

		template <typename T>
		void write(const uintptr_t address, const T& buffer)
		{
			WriteProcessMemory(address, uint64_t(&buffer), sizeof(T));
		}

		/*
				template <typename T>
				void write(const uintptr_t address, T value)
				{
					auto buffer = value;
					WriteProcessMemory(address, uint64_t(&buffer), sizeof(T));
				}*/

		template<typename T>
		inline bool read_array(uint64_t address, T* array, size_t len) {
			return ReadProcessMemory((PVOID)address, (PVOID)array, sizeof(T) * len);
		}

	}
}