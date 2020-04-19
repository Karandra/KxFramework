#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include "Kx/FileSystem/FSPath.h"
#include "Kx/Localization/Locale.h"

namespace KxFramework
{
	class DynamicLibrarySearchPath;

	enum class DynamicLibraryLoadFlag
	{
		None = 0,

		DataFile = 1 << 0,
		ImageResource = 1 << 1,
		SearchUserDirectories = 1 << 2,

		Exclusive = 1 << 16
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(DynamicLibraryLoadFlag);
	}
}

namespace KxFramework
{
	class KX_API DynamicLibrary final
	{
		public:
			static DynamicLibrary GetCurrentModule() noexcept;
			static DynamicLibrary GetLoadedModule(const String& name) noexcept;

			static DynamicLibrarySearchPath* AddSearchDirectory(const FSPath& path);
			static bool RemoveSearchDirectory(DynamicLibrarySearchPath& handle);

		private:
			std::optional<void*> m_Handle;
			DynamicLibraryLoadFlag m_LoadFlags = DynamicLibraryLoadFlag::None;
			bool m_ShouldUnload = false;

		public:
			DynamicLibrary() noexcept = default;
			DynamicLibrary(const FSPath& path, DynamicLibraryLoadFlag flags = DynamicLibraryLoadFlag::None)
			{
				Load(path, flags);
			}
			DynamicLibrary(DynamicLibrary&& other) noexcept
			{
				*this = std::move(other);
			}
			DynamicLibrary(const DynamicLibrary&) = delete;
			~DynamicLibrary() noexcept
			{
				Unload();
			}

		public:
			// General
			bool IsNull() const noexcept
			{
				return !m_Handle || m_Handle == nullptr;
			}
			bool ShouldUnload() const noexcept
			{
				return !IsNull() && m_ShouldUnload;
			}
			void* GetHandle() const noexcept
			{
				return m_Handle.value_or(nullptr);
			}
			FSPath GetFilePath() const;

			bool Load(const FSPath& path, DynamicLibraryLoadFlag flags = DynamicLibraryLoadFlag::None);
			void Unload();

			void AttachHandle(void* handle, bool takeOwnership = false) noexcept
			{
				Unload();

				m_Handle = handle;
				m_ShouldUnload = takeOwnership;
				m_LoadFlags = DynamicLibraryLoadFlag::None;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle.value_or(nullptr);
				m_Handle = {};
				m_ShouldUnload = false;
				m_LoadFlags = DynamicLibraryLoadFlag::None;

				return handle;
			}

			// Functions
			size_t EnumFunctionNames(std::function<bool(String)> func) const;

			void* GetFunctionAddress(const char* name) const;
			void* GetFunctionAddress(const wchar_t* name) const;
			void* GetFunctionAddress(const String& name) const
			{
				return GetFunctionAddress(name.wx_str());
			}
			void* GetFunctionAddress(size_t ordinal) const;

			template<class T, class TFunc>
			TFunc* GetFunction(T&& name) const
			{
				static_assert(std::is_function_v<std::remove_pointer_t<std::remove_reference_t<TFunc>>>);

				return reinterpret_cast<TFunc*>(GetFunctionAddress(std::forward<T>(name)));
			}

			// Resources
			bool IsDataFile() const noexcept;
			bool IsImageResource() const noexcept;
			bool IsAnyResource() const noexcept;

			
			bool IsResourceExist(const String& resType, const String& resName, const Locale& locale = {}) const;
			size_t EnumResourceTypes(std::function<bool(String)> func, const Locale& locale = {}) const;
			size_t EnumResourceNames(const String& resType, std::function<bool(String)> func, const Locale& locale = {}) const;
			size_t EnumResourceLanguages(const String& resType, const String& resName, std::function<bool(Locale)> func) const;
			wxScopedCharBuffer GetResource(const String& resType, const String& resName, const Locale& locale = {}) const;

			wxBitmap GetBitmapResource(const String& name, const Locale& locale = {}) const;
			wxIcon GetIconResource(const String& name, const wxSize& size = wxDefaultSize, const Locale& locale = {}) const;
			wxIcon GetIconResource(const String& name, size_t index, const Locale& locale = {}) const;
			size_t GetIconResourceCount(const String& name, const Locale& locale = {}) const;
			wxCursor GetCursorResource(const String& name, const Locale& locale = {}) const;
			String GetStringResource(const String& name, const Locale& locale = {}) const;
			String GetMessageResource(uint32_t messageID, const Locale& locale = {}) const;

		public:
			DynamicLibrary& operator=(DynamicLibrary&& other) noexcept
			{
				m_Handle = std::move(m_Handle);
				
				m_LoadFlags = other.m_LoadFlags;
				other.m_LoadFlags = DynamicLibraryLoadFlag::None;

				m_ShouldUnload = other.m_ShouldUnload;
				other.m_ShouldUnload = false;

				return *this;
			}
			DynamicLibrary& operator=(const DynamicLibrary&) = delete;

			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
