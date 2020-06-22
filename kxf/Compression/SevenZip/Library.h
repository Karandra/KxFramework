#pragma once
#include "Common.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/General/Singleton.h"
#include "kxf/System/COM.h"
struct IUnknown;

namespace kxf::SevenZip
{
	class KX_API Library final: public Singleton<Library>
	{
		friend class Singleton<Library>;
		
		private:
			DynamicLibrary m_Library;
			void* m_CreateObjectFunc = nullptr;

		private:
			Library() = default;
			~Library()
			{
				Unload();
			}

		public:
			bool IsLoaded() const noexcept
			{
				return m_Library && m_CreateObjectFunc;
			}
			DynamicLibrary& GetLibrary() noexcept
			{
				return m_Library;
			}

			bool Load();
			bool Load(const FSPath& libraryPath);
			void Unload() noexcept;

			bool CreateObject(const NativeUUID& classID, const NativeUUID& interfaceID, void** object) const noexcept;

			template<class T>
			COMPtr<T> CreateObject(const NativeUUID& classID, const NativeUUID& interfaceID) const noexcept
			{
				static_assert(std::is_base_of_v<IUnknown, T>, "Must be COM class");

				COMPtr<T> object;
				if (CreateObject(classID, interfaceID, object.GetAddress()))
				{
					return object;
				}
				return nullptr;
			}
	};
}
