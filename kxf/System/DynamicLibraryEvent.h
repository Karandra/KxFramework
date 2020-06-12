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
	class KX_API DynamicLibraryEvent: public wxNotifyEvent
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
			DynamicLibraryEvent(EventID eventType)
				:wxNotifyEvent(eventType)
			{
			}

		public:
			DynamicLibraryEvent* Clone() const override
			{
				return new DynamicLibraryEvent(*this);
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

		public:
			wxDECLARE_DYNAMIC_CLASS(DynamicLibraryEvent);
	};
}
