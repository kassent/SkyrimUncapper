#include "Utilities.h"
#include "Shlwapi.h"
#include <string>
#include <memory>

#pragma comment(lib, "shlwapi.lib")

std::string GetRuntimePath()
{
	static char	appPath[4096] = { 0 };

	if(appPath[0])
		return appPath;

	ASSERT(GetModuleFileName(GetModuleHandle(NULL), appPath, sizeof(appPath)));

	return appPath;
}

std::string GetRuntimeName()
{
	std::string appPath = GetRuntimePath();

	std::string::size_type slashOffset = appPath.rfind('\\');
	if(slashOffset == std::string::npos)
		return appPath;

	return appPath.substr(slashOffset + 1);
}

const std::string & GetRuntimeDirectory()
{
	static std::string s_runtimeDirectory;

	if(s_runtimeDirectory.empty())
	{
		std::string	runtimePath = GetRuntimePath();

		// truncate at last slash
		std::string::size_type	lastSlash = runtimePath.rfind('\\');
		if(lastSlash != std::string::npos)	// if we don't find a slash something is VERY WRONG
		{
			s_runtimeDirectory = runtimePath.substr(0, lastSlash + 1);
		}
		else
		{
			_WARNING("no slash in runtime path? (%s)", runtimePath.c_str());
		}
	}

	return s_runtimeDirectory;
}

std::string GetCurrentDLLDirectory()
{
	TCHAR buffer[MAX_PATH];
	HMODULE hModule;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPTSTR)GetCurrentDLLDirectory, &hModule);
	if (hModule != nullptr)
	{
		GetModuleFileName(hModule, buffer, MAX_PATH);
		PathRemoveFileSpec(buffer);
		std::string result(buffer);
		return std::move(result);
	}
	return "";
}

const std::string & GetConfigPath()
{
	static std::string s_configPath;

	if(s_configPath.empty())
	{
		std::string	runtimePath = GetRuntimeDirectory();
		if(!runtimePath.empty())
		{
			s_configPath = runtimePath + "Data\\SKSE\\Plugin\\SkyrimUncapper.ini";

			_MESSAGE("config path = %s", s_configPath.c_str());
		}
	}

	return s_configPath;
}

std::string GetConfigOption(const char * section, const char * key)
{
	std::string	result;

	const std::string & configPath = GetConfigPath();
	if(!configPath.empty())
	{
		char	resultBuf[256];
		resultBuf[0] = 0;

		UInt32	resultLen = GetPrivateProfileString(section, key, NULL, resultBuf, sizeof(resultBuf), configPath.c_str());

		result = resultBuf;
	}

	return result;
}

bool GetConfigOption_UInt32(const char * section, const char * key, UInt32 * dataOut)
{
	std::string	data = GetConfigOption(section, key);
	if(data.empty())
		return false;

	return (sscanf_s(data.c_str(), "%u", dataOut) == 1);
}

const std::string & GetOSInfoStr()
{
	static std::string	result;

	if(result.empty())
	{
		OSVERSIONINFO	info;

		info.dwOSVersionInfoSize = sizeof(info);

		if(GetVersionEx(&info))
		{
			char	buf[256];

			sprintf_s(buf, sizeof(buf), "%d.%d (%d)", info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber);

			result = buf;
		}
		else
		{
			result = "unknown";
		}
	}

	return result;
}

void * GetIATAddr(void * module, const char * searchDllName, const char * searchImportName)
{
	UInt8					* base = (UInt8 *)module;
	IMAGE_DOS_HEADER		* dosHeader = (IMAGE_DOS_HEADER *)base;
	IMAGE_NT_HEADERS		* ntHeader = (IMAGE_NT_HEADERS *)(base + dosHeader->e_lfanew);
	IMAGE_IMPORT_DESCRIPTOR	* importTable =
		(IMAGE_IMPORT_DESCRIPTOR *)(base + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	for(; importTable->Characteristics; ++importTable)
	{
		const char	* dllName = (const char *)(base + importTable->Name);

		if(!_stricmp(dllName, searchDllName))
		{
			// found the dll

			IMAGE_THUNK_DATA	* thunkData = (IMAGE_THUNK_DATA *)(base + importTable->OriginalFirstThunk);
			uintptr_t			* iat = (uintptr_t *)(base + importTable->FirstThunk);

			for(; thunkData->u1.Ordinal; ++thunkData, ++iat)
			{
				if(!IMAGE_SNAP_BY_ORDINAL(thunkData->u1.Ordinal))
				{
					IMAGE_IMPORT_BY_NAME	* importInfo = (IMAGE_IMPORT_BY_NAME *)(base + thunkData->u1.AddressOfData);

					if(!_stricmp((char *)importInfo->Name, searchImportName))
					{
						// found the import
						return iat;
					}
				}
			}

			return NULL;
		}
	}

	return NULL;
}
/*
uintptr_t BinarySearch(const std::vector<UInt8>& binary)
{
	uintptr_t result = NULL;
	static UInt8			* base = (UInt8 *)GetModuleHandle(NULL);
	IMAGE_DOS_HEADER		* dosHeader = (IMAGE_DOS_HEADER *)base;
	IMAGE_NT_HEADERS		* ntHeader = (IMAGE_NT_HEADERS *)(base + dosHeader->e_lfanew);
	DWORD baseOfCode = ntHeader->OptionalHeader.BaseOfCode;
	DWORD sizeOfCode = ntHeader->OptionalHeader.SizeOfCode;
	uintptr_t startPoint = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)) + baseOfCode;
	uintptr_t endPoint = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)) + baseOfCode + sizeOfCode;

	ASSERT(startPoint <= endPoint);
	size_t binarySize = binary.size();			//KMP

	std::unique_ptr<UInt32[]> next(new UInt32[binarySize]);
	SInt32 k = -1, j = 0;
	next[0] = -1;
	while (j < binarySize - 1)
	{
		if (k == -1 || binary[j] == binary[k])
		{
			if (binary[++j] == binary[++k])
				next[j] = next[k];
			else
				next[j] = k;
		}
		else
			k = next[k];
	}
	_MESSAGE("Create next array,start: %016X, endPoint: %016X", startPoint, endPoint);
	while (startPoint <= endPoint - binarySize)
	{
		for (size_t i = 0; i < binarySize; ++i)
		{
			UInt32 oldProtect;
			VirtualProtect((void *)(i + startPoint), sizeof(UInt8), PAGE_EXECUTE_READWRITE, &oldProtect);
			UInt8 code = *reinterpret_cast<UInt8*>(i + startPoint);
			VirtualProtect((void *)(i + startPoint), sizeof(UInt8), oldProtect, &oldProtect);

			if (binary[i] == code)
			{
				if (i == binarySize - 1)
				{
					result = startPoint;
					_MESSAGE("Find code in memory,addr is %016X, offset is: %08X", result, result - (uintptr_t)base);
					return result;
				}
			}
			else
			{
				startPoint += (i - next[i]);
				break;
			}
		}
	}
	_MESSAGE("Finish....");
	return result;
}
*/

#pragma warning (push)
#pragma warning (disable : 4200)
struct RTTIType
{
	void	* typeInfo;
	UInt64	data;
	char	name[0];
};

struct RTTILocator
{
	UInt32		sig, offset, cdOffset;
	UInt32		typeDesc;
	UInt32		classDesc;
};
#pragma warning (pop)

// use the RTTI information to return an object's class name
const char * GetObjectClassName(void * objBase)
{
	const char	* result = "<no rtti>";
	__try
	{
		void		** obj = (void **)objBase;
		RTTILocator	** vtbl = (RTTILocator **)obj[0];
		RTTILocator	* rtti = vtbl[-1];
		UInt64 typeDesc = rtti->typeDesc;
		RelocPtr<RTTIType> type(typeDesc);

		// starts with ,?
		if((type->name[0] == '.') && (type->name[1] == '?'))
		{
			// is at most 100 chars long
			for(UInt32 i = 0; i < 100; i++)
			{
				if(type->name[i] == 0)
				{
					// remove the .?AV
					result = type->name + 4;
					break;
				}
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// return the default
	}

	return result;
}

void DumpClass(void * theClassPtr, UInt64 nIntsToDump)
{
	UInt64* basePtr = (UInt64*)theClassPtr;

	_MESSAGE("DumpClass: %016I64X", basePtr);

	gLog.Indent();

	if (!theClassPtr) return;
	for (UInt64 ix = 0; ix < nIntsToDump; ix++ ) {
		UInt64* curPtr = basePtr+ix;
		const char* curPtrName = NULL;
		UInt64 otherPtr = 0;
		float otherFloat1 = 0.0;
		float otherFloat2 = 0.0;
		const char* otherPtrName = NULL;
		if (curPtr) {
			curPtrName = GetObjectClassName((void*)curPtr);

			__try
			{
				otherPtr = *curPtr;
				UInt32 lowerFloat = otherPtr & 0xFFFFFFFF;
				UInt32 upperFloat = (otherPtr >> 32) & 0xFFFFFFFF;
				otherFloat1 = *(float*)&lowerFloat;
				otherFloat2 = *(float*)&upperFloat;
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				//
			}

			if (otherPtr) {
				otherPtrName = GetObjectClassName((void*)otherPtr);
			}
		}

		_MESSAGE("%3d +%03X ptr: 0x%016I64X: %32s *ptr: 0x%016I64x | %f, %f: %32s", ix, ix*8, curPtr, curPtrName, otherPtr, otherFloat2, otherFloat1, otherPtrName);
	}

	gLog.Outdent();
}