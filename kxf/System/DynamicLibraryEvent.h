#pragma once
#include "Common.h"
#include "kxf/EventSystem/Event.h"
#include "kxf/FileSystem/FSPath.h"

namespace kxf
{
	class DynamicLibrary;
}

namespace kxf
{
	class KX_API DynamicLibraryEvent: public BasicEvent
	{
		public:
			KxEVENT_MEMBER(DynamicLibraryEvent, Loaded);
			KxEVENT_MEMBER(DynamicLibraryEvent, Unloaded);

		private:
			void* m_Handle = nullptr;
			FSPath m_BaseName;
			FSPath m_FullPath;

		public:
			DynamicLibraryEvent() = default;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<DynamicLibraryEvent>(std::move(*this));
			}
			
			DynamicLibrary GetLibrary() const;
			void SetLibrary(void* handle)
			{
				m_Handle = handle;
			}

			FSPath GetBaseName() const
			{
				return m_BaseName;
			}
			void SetBaseName(FSPath baseName)
			{
				m_BaseName = std::move(baseName);
			}

			FSPath GetFullPath() const
			{
				return m_FullPath;
			}
			void SetFullPath(FSPath fullPath)
			{
				m_FullPath = std::move(fullPath);
			}
	};
}
