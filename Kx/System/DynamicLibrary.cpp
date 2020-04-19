#include "stdafx.h"
#include "DynamicLibrary.h"
#include "Kx/System/NativeAPI.h"
#include "Kx/System/Private/System.h"
#include "Kx/System/Private/BinaryResourceDefines.h"
#include "Kx/FileSystem/FileStream.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/CallAtScopeExit.h"

#include <DbgHelp.h>
#include <cwchar>
#include "Kx/System/UndefWindows.h"
#pragma comment(lib, "DbgHelp.lib")

namespace
{
	HMODULE AsHMODULE(void* handle) noexcept
	{
		return reinterpret_cast<HMODULE>(handle);
	}
	LPCWSTR GetNameOrID(const KxFramework::String& name)
	{
		if (auto id = name.ToInt<ULONG>())
		{
			return MAKEINTRESOURCEW(*id);
		}
		return name.wc_str();
	}
	DWORD GetLangID(const KxFramework::Locale& locale) noexcept
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

	constexpr DWORD MapDynamicLibraryLoadFlags(KxFramework::DynamicLibraryLoadFlag flags) noexcept
	{
		using namespace KxFramework;

		DWORD nativeFlags = 0;
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_AS_DATAFILE, flags & DynamicLibraryLoadFlag::DataFile);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE, flags & DynamicLibraryLoadFlag::DataFile && flags & DynamicLibraryLoadFlag::Exclusive);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_AS_IMAGE_RESOURCE, flags & DynamicLibraryLoadFlag::ImageResource);
		Utility::AddFlagRef(nativeFlags, LOAD_LIBRARY_SEARCH_USER_DIRS, flags & DynamicLibraryLoadFlag::SearchUserDirectories);

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

	const wxSize g_DefaultIconSize = wxSize(0, 0);

	HRSRC GetResourceHandle(HMODULE handle, const KxFramework::String& type, const KxFramework::String& name, const KxFramework::Locale& locale)
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

	HANDLE DoLoadGDIImage(HMODULE handle, const KxFramework::String& name, const KxFramework::String& type, UINT gdiType, wxSize size, const KxFramework::Locale& locale)
	{
		if (HGLOBAL resDataHandle = ::LoadResource(handle, GetResourceHandle(handle, type, name, locale)))
		{
			return LoadImageW(handle, GetNameOrID(name), gdiType, size.GetWidth(), size.GetHeight(), LR_DEFAULTCOLOR);
		}
		return nullptr;
	}
	
	template<class T>
	T LoadGDIImage(HMODULE handle, const KxFramework::String& name, const KxFramework::String& type, UINT gdiType, wxSize size, const KxFramework::Locale& locale)
	{
		if (HANDLE imageHandle = DoLoadGDIImage(handle, name, type, gdiType, size, locale))
		{
			T image;
			image.SetHandle(imageHandle);
			if (size.IsFullySpecified())
			{
				// These functions were removed in wxWidgets 3.1.2
				//image.SetWidth(size.GetWidth());
				//image.SetHeight(size.GetHeight());
			}
			return image;
		}
		return {};
	}
	
	template<>
	wxIcon LoadGDIImage<wxIcon>(HMODULE handle, const KxFramework::String& name, const KxFramework::String& type, UINT gdiType, wxSize size, const KxFramework::Locale& locale)
	{
		if (HANDLE imageHandle = DoLoadGDIImage(handle, name, type, gdiType, size, locale))
		{
			wxIcon icon;
			icon.CreateFromHICON(static_cast<HICON>(imageHandle));
			return icon;
		}
		return {};
	}
}

namespace KxFramework
{
	DynamicLibrary DynamicLibrary::GetCurrentModule() noexcept
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

	DynamicLibrarySearchPath* DynamicLibrary::AddSearchDirectory(const FSPath& path)
	{
		if (NativeAPI::Kernel32::AddDllDirectory)
		{
			String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);
			return reinterpret_cast<DynamicLibrarySearchPath*>(NativeAPI::Kernel32::AddDllDirectory(pathString.wc_str()));
		}
		return nullptr;
	}
	bool DynamicLibrary::RemoveSearchDirectory(DynamicLibrarySearchPath& handle)
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

	bool DynamicLibrary::Load(const FSPath& path, DynamicLibraryLoadFlag flags)
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
	void DynamicLibrary::Unload()
	{
		if (!IsNull() && m_ShouldUnload)
		{
			::FreeLibrary(AsHMODULE(*m_Handle));
		}
		m_Handle = {};
		m_LoadFlags = DynamicLibraryLoadFlag::None;
		m_ShouldUnload = false;
	}

	// Functions
	size_t DynamicLibrary::EnumFunctionNames(std::function<bool(String)> func) const
	{
		#pragma warning(push, 0)
		#pragma warning(disable: 4302)
		#pragma warning(disable: 4311)
		#pragma warning(disable: 4312)

		FileStream stream(GetFilePath(), FileStreamAccess::Read, FileStreamDisposition::OpenExisting, FileStreamShare::Read);
		if (stream)
		{
			if (HANDLE fileMapping = ::CreateFileMappingW(stream.GetHandle(), nullptr, PAGE_READONLY, 0, 0, nullptr))
			{
				Utility::CallAtScopeExit closeMapping = [&]()
				{
					::CloseHandle(fileMapping);
				};

				if (void* fileBase = ::MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0))
				{
					Utility::CallAtScopeExit unmapView = [&]()
					{
						::UnmapViewOfFile(fileBase);
					};

					auto headerDOS = reinterpret_cast<IMAGE_DOS_HEADER*>(fileBase);
					auto headerNT = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<size_t>(headerDOS) + static_cast<size_t>(headerDOS->e_lfanew));
					if (!(::IsBadReadPtr(headerNT, sizeof(IMAGE_NT_HEADERS)) || headerNT->Signature != IMAGE_NT_SIGNATURE))
					{
						if (auto exportDir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(headerNT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress))
						{
							exportDir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(::ImageRvaToVa(headerNT, headerDOS, (size_t)exportDir, nullptr));

							auto namesArray = reinterpret_cast<DWORD**>(exportDir->AddressOfNames);
							namesArray = reinterpret_cast<DWORD**>(::ImageRvaToVa(headerNT, headerDOS, reinterpret_cast<size_t>(namesArray), nullptr));

							if (namesArray)
							{
								size_t count = 0;
								for (size_t i = 0; i < static_cast<size_t>(exportDir->NumberOfNames); i++)
								{
									#pragma warning(suppress: 4302)
									#pragma warning(suppress: 4311)
									auto name = reinterpret_cast<const char*>(::ImageRvaToVa(headerNT, headerDOS, reinterpret_cast<DWORD>(namesArray[i]), nullptr));

									count++;
									if (!std::invoke(func, name))
									{
										break;
									}
								}
								return count;
							}
						}
					}
				}
			}
		}
		return 0;

		#pragma warning(pop)
	}
	
	void* DynamicLibrary::GetFunctionAddress(const char* name) const
	{
		if (m_Handle)
		{
			return ::GetProcAddress(AsHMODULE(*m_Handle), name);
		}
		return nullptr;
	}
	void* DynamicLibrary::GetFunctionAddress(const wchar_t* name) const
	{
		if (m_Handle)
		{
			String temp = name;
			return ::GetProcAddress(AsHMODULE(*m_Handle), temp.c_str());
		}
		return nullptr;
	}
	void* DynamicLibrary::GetFunctionAddress(size_t ordinal) const
	{
		if (m_Handle)
		{
			return ::GetProcAddress(AsHMODULE(*m_Handle), MAKEINTRESOURCEA(ordinal));
		}
		return nullptr;
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

	wxBitmap DynamicLibrary::GetBitmapResource(const String& name, const Locale& locale) const
	{
		return LoadGDIImage<wxBitmap>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_BITMAP), IMAGE_BITMAP, g_DefaultIconSize, locale);
	}
	wxIcon DynamicLibrary::GetIconResource(const String& name, const wxSize& size, const Locale& locale) const
	{
		return LoadGDIImage<wxIcon>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_GROUP_ICON), IMAGE_ICON, size, locale);
	}
	wxIcon DynamicLibrary::GetIconResource(const String& name, size_t index, const Locale& locale) const
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

				// I don't remember what '0x00030000' is. WHY DIDN'T I ADD A LINK TO WHERE i FOUND THIS?!
				constexpr DWORD dwVer = 0x00030000u;
				if (HICON iconHandle = ::CreateIconFromResourceEx(reinterpret_cast<BYTE*>(iconBuffer.data()), iconBuffer.length(), TRUE, dwVer, width, height, LR_DEFAULTCOLOR))
				{
					wxIcon icon;
					icon.CreateFromHICON(iconHandle);
					return icon;
				}
			}
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
	wxCursor DynamicLibrary::GetCursorResource(const String& name, const Locale& locale) const
	{
		return LoadGDIImage<wxCursor>(AsHMODULE(*m_Handle), name, System::Private::ResourceTypeToName(RT_CURSOR), IMAGE_CURSOR, g_DefaultIconSize, locale);
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
