#include "stdafx.h"
#include "DynamicLibrary.h"
#include "kxf/System/NativeAPI.h"
#include "kxf/System/Private/System.h"
#include "kxf/System/Private/BinaryResourceDefines.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/INativeStream.h"
#include "kxf/Drawing/ImageBundle.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/Drawing/GDIRenderer/GDICursor.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/CallAtScopeExit.h"

#include <Windows.h>
#include <DbgHelp.h>
#include <cwchar>
#include "UndefWindows.h"
#pragma comment(lib, "DbgHelp.lib")

namespace
{
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;

	using namespace kxf;

	HMODULE AsHMODULE(void* handle) noexcept
	{
		return reinterpret_cast<HMODULE>(handle);
	}
	LPCWSTR GetNameOrID(const String& name)
	{
		if (auto id = name.ToInt<ULONG>())
		{
			return MAKEINTRESOURCEW(*id);
		}
		return name.wc_str();
	}
	DWORD GetLangID(const Locale& locale) noexcept
	{
		if (auto langID = locale.GetLangID())
		{
			return langID->ID;
		}
		return MAKELANGID(LANG_NEUTRAL, SORT_DEFAULT);
	}

	bool IsHandleDataFile(void* handle) noexcept
	{
		return reinterpret_cast<ULONG_PTR>(handle) & static_cast<ULONG_PTR>(1);
	}
	bool IsHandleImageResourse(void* handle) noexcept
	{
		return reinterpret_cast<ULONG_PTR>(handle) & static_cast<ULONG_PTR>(2);
	}

	constexpr uint32_t MapDynamicLibraryLoadFlags(FlagSet<DynamicLibraryFlag> flags) noexcept
	{
		uint32_t nativeFlags = 0;
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_AS_DATAFILE, flags & DynamicLibraryFlag::Resource);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE, flags & DynamicLibraryFlag::Resource && flags & DynamicLibraryFlag::Exclusive);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_AS_IMAGE_RESOURCE, flags & DynamicLibraryFlag::ImageResource);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_SEARCH_USER_DIRS, flags & DynamicLibraryFlag::SearchUserDirectories);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_SEARCH_SYSTEM32, flags & DynamicLibraryFlag::SearchSystemDirectories);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR, flags & DynamicLibraryFlag::SearchLibraryDirectory);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_SEARCH_APPLICATION_DIR, flags & DynamicLibraryFlag::SearchApplicationDirectory);

		return nativeFlags;
	}

	template<class TFunc>
	struct CallContext final
	{
		TFunc& Callable;
		size_t CallCount = 0;

		CallContext(TFunc& func)
			:Callable(func)
		{
		}
	};

	template<class TFunc>
	void MapDLL(void* handle, TFunc&& func)
	{
		// The unfortunate interaction between LOAD_LIBRARY_AS_DATAFILE and DialogBox
		// https://devblogs.microsoft.com/oldnewthing/20051006-09/?p=33883
		const auto base = reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(handle) & ~(static_cast<uintptr_t>(0xFFFF)));

		auto dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
		auto ntHeader = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base +  dosHeader->e_lfanew);
		auto sections = IMAGE_FIRST_SECTION(ntHeader);
		auto AdjustRVA = [&](uintptr_t rva) -> uintptr_t
		{
			for (size_t i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
			{
				const auto section = sections[i];
				if (section.VirtualAddress <= rva && rva < section.VirtualAddress + section.Misc.VirtualSize)
				{
					return rva - section.VirtualAddress + section.PointerToRawData;
				}
			}
			return 0;
		};

		func(base, dosHeader, ntHeader, sections, AdjustRVA);
	}

	constexpr Size g_DefaultIconSize = {0, 0};

	HRSRC GetResourceHandle(HMODULE handle, const String& type, const String& name, const Locale& locale)
	{
		return ::FindResourceExW(handle, GetNameOrID(type), GetNameOrID(name), GetLangID(locale));
	}
	wxScopedCharBuffer GetResourceData(HMODULE handle, HRSRC resHandle)
	{
		if (resHandle)
		{
			if (HGLOBAL resDataHandle = ::LoadResource(handle, resHandle))
			{
				void* resData = ::LockResource(resDataHandle);
				DWORD resSize = ::SizeofResource(handle, resHandle);
				if (resData && resSize != 0)
				{
					return wxScopedCharBuffer::CreateNonOwned(reinterpret_cast<const char*>(resData), resSize);
				}
			}
		}
		return {};
	}

	HANDLE DoLoadGDIImage(HMODULE handle, const String& name, const String& type, UINT gdiType, Size size, const Locale& locale)
	{
		if (HGLOBAL resDataHandle = ::LoadResource(handle, GetResourceHandle(handle, type, name, locale)))
		{
			return ::LoadImageW(handle, GetNameOrID(name), gdiType, size.GetWidth(), size.GetHeight(), LR_DEFAULTCOLOR);
		}
		return nullptr;
	}

	template<class T>
	T LoadGDIImage(HMODULE handle, const String& name, const String& type, UINT gdiType, Size size, const Locale& locale)
	{
		if (HANDLE imageHandle = DoLoadGDIImage(handle, name, type, gdiType, size, locale))
		{
			T image{};
			image.AttachHandle(imageHandle);
			return image;
		}
		return {};
	}
}

namespace kxf
{
	DynamicLibrary DynamicLibrary::GetCurrentModule() noexcept
	{
		// Retrieve the HMODULE for the current DLL or EXE using this symbol that the linker provides for every module

		DynamicLibrary library;
		library.m_Handle = reinterpret_cast<void*>(&__ImageBase);
		library.m_ShouldUnload = false;

		return library;
	}
	DynamicLibrary DynamicLibrary::GetExecutingModule() noexcept
	{
		DynamicLibrary library;
		library.m_Handle = reinterpret_cast<void*>(::GetModuleHandleW(nullptr));
		library.m_ShouldUnload = false;

		return library;
	}
	DynamicLibrary DynamicLibrary::GetLoadedModule(const String& name) noexcept
	{
		HMODULE handle = nullptr;
		if (::GetModuleHandleExW(0, name.wc_str(), &handle))
		{
			DynamicLibrary library;
			library.m_Handle = reinterpret_cast<void*>(handle);
			library.m_ShouldUnload = false;

			return library;
		}
		return {};
	}

	DynamicLibrary::SearchDirectory* DynamicLibrary::AddSearchDirectory(const FSPath& path)
	{
		if (NativeAPI::Kernel32::AddDllDirectory)
		{
			String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);
			return reinterpret_cast<SearchDirectory*>(NativeAPI::Kernel32::AddDllDirectory(pathString.wc_str()));
		}
		return nullptr;
	}
	bool DynamicLibrary::RemoveSearchDirectory(SearchDirectory& handle)
	{
		if (NativeAPI::Kernel32::RemoveDllDirectory)
		{
			return NativeAPI::Kernel32::RemoveDllDirectory(reinterpret_cast<DLL_DIRECTORY_COOKIE>(&handle));
		}
		return false;
	}

	// General
	FSPath DynamicLibrary::GetFilePath() const
	{
		if (m_Handle)
		{
			wchar_t buffer[INT16_MAX] = {};
			if (::GetModuleFileNameW(AsHMODULE(*m_Handle), buffer, std::size(buffer)) != 0)
			{
				return buffer;
			}
		}
		return {};
	}

	bool DynamicLibrary::Load(const FSPath& path, FlagSet<DynamicLibraryFlag> flags)
	{
		Unload();

		if (path)
		{
			String pathString = path.IsAbsolute() ? path.GetFullPathWithNS(FSPathNamespace::Win32File) : path.GetFullPath();
			m_Handle = reinterpret_cast<void*>(::LoadLibraryExW(pathString.wc_str(), nullptr, MapDynamicLibraryLoadFlags(flags)));
			m_LoadFlags = flags;
			m_ShouldUnload = true;

			return !IsNull();
		}
		return false;
	}
	void DynamicLibrary::Unload() noexcept
	{
		if (!IsNull() && m_ShouldUnload)
		{
			::FreeLibrary(AsHMODULE(*m_Handle));
		}
		m_Handle = {};
		m_LoadFlags = DynamicLibraryFlag::None;
		m_ShouldUnload = false;
	}

	// Functions
	size_t DynamicLibrary::EnumExportedFunctionNames(std::function<bool(String)> func) const
	{
		size_t count = 0;
		MapDLL(*m_Handle, [&](const char* base, const IMAGE_DOS_HEADER* dosHeader, const IMAGE_NT_HEADERS64* ntHeader, const IMAGE_SECTION_HEADER* sections, auto& adjustRVA)
		{
			const auto exportDirectory = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			if (exportDirectory.Size != 0)
			{
				auto exports = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base + adjustRVA(exportDirectory.VirtualAddress));
				if (auto namesArray = reinterpret_cast<const uint32_t*>(reinterpret_cast<uintptr_t>(base) + adjustRVA(exports->AddressOfNames)))
				{
					for (size_t i = 0; i < static_cast<size_t>(exports->NumberOfNames); i++)
					{
						const auto name = reinterpret_cast<const char*>(base + adjustRVA(namesArray[i]));
						if (name && *name)
						{
							count++;
							if (!std::invoke(func, name))
							{
								break;
							}
						}
					}
				}
			}
		});
		return count;
	}

	void* DynamicLibrary::GetExportedFunctionAddress(const char* name) const
	{
		if (m_Handle)
		{
			return ::GetProcAddress(AsHMODULE(*m_Handle), name);
		}
		return nullptr;
	}
	void* DynamicLibrary::GetExportedFunctionAddress(const wchar_t* name) const
	{
		if (m_Handle)
		{
			String temp = name;
			return ::GetProcAddress(AsHMODULE(*m_Handle), temp.c_str());
		}
		return nullptr;
	}
	void* DynamicLibrary::GetExportedFunctionAddress(size_t ordinal) const
	{
		if (m_Handle)
		{
			return ::GetProcAddress(AsHMODULE(*m_Handle), MAKEINTRESOURCEA(ordinal));
		}
		return nullptr;
	}

	// Dependencies
	size_t DynamicLibrary::EnumDependencyModuleNames(std::function<bool(String)> func) const
	{
		if (m_Handle)
		{
			size_t count = 0;
			MapDLL(*m_Handle, [&](const char* base, const IMAGE_DOS_HEADER* dosHeader, const IMAGE_NT_HEADERS64* ntHeader, const IMAGE_SECTION_HEADER* sections, auto& adjustRVA)
			{
				auto importDirectory = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
				if (importDirectory.Size != 0)
				{
					auto imports = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(base + adjustRVA(importDirectory.VirtualAddress));
					for (auto it = imports; it->Characteristics != 0; ++it)
					{
						const auto name = reinterpret_cast<const char*>(base + adjustRVA(it->Name));
						if (name && *name)
						{
							count++;
							if (!std::invoke(func, name))
							{
								break;
							}
						}
					}
				}
			});
			return count;
		}
		return 0;
	}

	// Resources
	bool DynamicLibrary::IsDataFile() const noexcept
	{
		return m_Handle ? IsHandleDataFile(*m_Handle) : false;
	}
	bool DynamicLibrary::IsImageResource() const noexcept
	{
		return m_Handle ? IsHandleImageResourse(*m_Handle) : false;
	}
	bool DynamicLibrary::IsAnyResource() const noexcept
	{
		return IsDataFile() || IsImageResource();
	}

	bool DynamicLibrary::IsResourceExist(const String& resType, const String& resName, const Locale& locale) const
	{
		return m_Handle && GetResourceHandle(AsHMODULE(*m_Handle), resType, resName, locale) != nullptr;
	}
	size_t DynamicLibrary::EnumResourceTypes(std::function<bool(String)> func, const Locale& locale) const
	{
		if (m_Handle)
		{
			CallContext callContext(func);
			::EnumResourceTypesExW(AsHMODULE(*m_Handle), [](HMODULE handle, LPWSTR resType, LONG_PTR lParam) -> BOOL
			{
				auto& context = *reinterpret_cast<decltype(callContext)*>(lParam);

				context.CallCount++;
				return std::invoke(context.Callable, resType);
			}, reinterpret_cast<LONG_PTR>(&callContext), 0, GetLangID(locale));
			return callContext.CallCount;
		}
		return 0;
	}
	size_t DynamicLibrary::EnumResourceNames(const String& resType, std::function<bool(String)> func, const Locale& locale) const
	{
		if (m_Handle)
		{
			CallContext callContext(func);
			::EnumResourceNamesExW(AsHMODULE(*m_Handle), GetNameOrID(resType), [](HMODULE handle, LPCWSTR resType, LPWSTR resName, LONG_PTR lParam) -> BOOL
			{
				auto& context = *reinterpret_cast<decltype(callContext)*>(lParam);

				context.CallCount++;
				return std::invoke(context.Callable, resName);
			}, reinterpret_cast<LONG_PTR>(&callContext), 0, GetLangID(locale));
		}
		return 0;
	}
	size_t DynamicLibrary::EnumResourceLanguages(const String& resType, const String& resName, std::function<bool(Locale)> func) const
	{
		if (m_Handle)
		{
			CallContext callContext(func);
			::EnumResourceLanguagesExW(AsHMODULE(*m_Handle), GetNameOrID(resType), GetNameOrID(resName), [](HMODULE handle, LPCWSTR resType, LPCWSTR resName, WORD langID, LONG_PTR lParam) -> BOOL
			{
				auto& context = *reinterpret_cast<decltype(callContext)*>(lParam);

				context.CallCount++;
				return std::invoke(context.Callable, Locale::FromLangID(Localization::LangID(langID, SORT_DEFAULT)));
			}, reinterpret_cast<LONG_PTR>(&callContext), 0, 0);
			return callContext.CallCount;
		}
		return 0;
	}
	wxScopedCharBuffer DynamicLibrary::GetResource(const String& resType, const String& resName, const Locale& locale) const
	{
		if (m_Handle)
		{
			return GetResourceData(AsHMODULE(*m_Handle), GetResourceHandle(AsHMODULE(*m_Handle), resType, resName, locale));
		}
		return {};
	}
	String DynamicLibrary::GetMessageResource(uint32_t messageID, const Locale& locale) const
	{
		if (m_Handle)
		{
			return System::Private::FormatMessage(*m_Handle, messageID, FORMAT_MESSAGE_FROM_HMODULE, locale);
		}
		return {};
	}

	size_t DynamicLibrary::GetIconResourceCount(const String& name, const Locale& locale) const
	{
		wxScopedCharBuffer groupBuffer = GetResource(System::Private::ResourceTypeToName(RT_GROUP_ICON), name, locale);
		if (groupBuffer.length() != 0)
		{
			const auto* iconGroup =  reinterpret_cast<System::Private::IconGroupDirectory*>(groupBuffer.data());
			return iconGroup->idCount;
		}
		return 0;
	}
	GDIIcon DynamicLibrary::GetIconResource(const String& name, const Size& size, const Locale& locale) const
	{
		return LoadGDIImage<GDIIcon>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_GROUP_ICON), IMAGE_ICON, size, locale);
	}
	GDIIcon DynamicLibrary::GetIconResource(const String& name, size_t index, const Locale& locale) const
	{
		using System::Private::ResourceTypeToName;
		using System::Private::IconGroupDirectory;
		using System::Private::IconGroupEntry;

		wxScopedCharBuffer groupBuffer = GetResource(ResourceTypeToName(RT_GROUP_ICON), name, locale);
		if (groupBuffer.length() != 0)
		{
			IconGroupDirectory* iconGroup = reinterpret_cast<IconGroupDirectory*>(groupBuffer.data());

			const size_t iconCount = iconGroup->idCount;
			if (index >= iconCount)
			{
				index = iconCount - 1;
			}

			IconGroupEntry* iconInfo = &iconGroup->idEntries[index];
			const size_t imageID = iconInfo->id;

			auto iconBuffer = GetResourceData(AsHMODULE(*m_Handle), GetResourceHandle(AsHMODULE(*m_Handle), ResourceTypeToName(RT_ICON), ResourceTypeToName(imageID), locale));
			if (iconBuffer.length() != 0)
			{
				int width = 0;
				int height = 0;

				// I don't remember what '0x00030000' is. WHY DIDN'T I ADD A LINK TO WHERE I FOUND THIS?!
				constexpr DWORD dwVer = 0x00030000u;
				if (HICON iconHandle = ::CreateIconFromResourceEx(reinterpret_cast<BYTE*>(iconBuffer.data()), iconBuffer.length(), TRUE, dwVer, width, height, LR_DEFAULTCOLOR))
				{
					GDIIcon icon;
					icon.AttachHandle(iconHandle);
					return icon;
				}
			}
		}
		return {};
	}
	ImageBundle DynamicLibrary::GetIconBundleResource(const String& name, const Locale& locale) const
	{
		const size_t count = GetIconResourceCount(name, locale);

		ImageBundle bundle(count);
		for (size_t i = 0; i < count; i++)
		{
			bundle.AddImage(GetIconResource(name, i, locale).ToImage());
		}
		return bundle;
	}

	GDIBitmap DynamicLibrary::GetBitmapResource(const String& name, const Locale& locale) const
	{
		return LoadGDIImage<GDIBitmap>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_BITMAP), IMAGE_BITMAP, g_DefaultIconSize, locale);
	}
	GDICursor DynamicLibrary::GetCursorResource(const String& name, const Locale& locale) const
	{
		return LoadGDIImage<GDICursor>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_CURSOR), IMAGE_CURSOR, g_DefaultIconSize, locale);
	}
	String DynamicLibrary::GetStringResource(const String& name, const Locale& locale) const
	{
		if (m_Handle)
		{
			// LoadString can not load strings with non-default locale
			if (locale && !locale.IsInvariant())
			{
				// http://forum.sources.ru/index.php?showtopic=375357
				if (auto stringID = name.ToInt<size_t>())
				{
					wxScopedCharBuffer data = GetResource(System::Private::ResourceTypeToName(RT_STRING), System::Private::ResourceTypeToName(*stringID / 16 + 1), locale);
					if (data.length() != 0)
					{
						stringID = *stringID % 16;
						const size_t tableSize = data.length() / sizeof(wchar_t);
						size_t offset = 0;
						size_t index = 0;

						const wchar_t* stringData = reinterpret_cast<const wchar_t*>(data.data());
						while (offset < tableSize)
						{
							if (index == *stringID)
							{
								size_t stringLength = stringData[offset];
								if (stringLength > 0)
								{
									return String(&(stringData[offset + 1]), stringLength);
								}
								break;
							}

							offset += stringData[offset] + 1;
							index++;
						}
					}
				}
			}
			else
			{
				if (auto id = name.ToInt<int>())
				{
					LPWSTR string = nullptr;
					int length = LoadStringW(AsHMODULE(*m_Handle), *id, reinterpret_cast<LPWSTR>(&string), 0);
					if (length > 0)
					{
						return String(string, length);
					}
				}
			}
		}
		return {};
	}
}
