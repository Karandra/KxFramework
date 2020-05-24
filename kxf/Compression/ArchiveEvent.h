#pragma once
#include "Common.h"
#include "kxf/FileSystem/FileOperationEvent.h"
#include "kxf/FileSystem/FileItem.h"
class wxInputStream;
class wxOutputStream;

namespace kxf
{
	class KX_API ArchiveEvent: public FileOperationEvent
	{
		public:
			KxEVENT_MEMBER(ArchiveEvent, Process);
			KxEVENT_MEMBER(ArchiveEvent, Search);
			KxEVENT_MEMBER(ArchiveEvent, Done);

			KxEVENT_MEMBER(ArchiveEvent, GetInputStream);
			KxEVENT_MEMBER(ArchiveEvent, GetOutputStream);

		private:
			String m_Method;
			String m_Attributes;
			FileItem m_FileItem;

			wxInputStream* m_InputStream = nullptr;
			wxOutputStream* m_OutputStream = nullptr;

		public:
			ArchiveEvent(EventID type = Event::EvtNull, int id = 0)
				:FileOperationEvent(type, id)
			{
			}

		public:
			ArchiveEvent* Clone() const override
			{
				return new ArchiveEvent(*this);
			}
			
			const FileItem& GetItem() const
			{
				return m_FileItem;
			}
			FileItem& GetItem()
			{
				return m_FileItem;
			}

			uint32_t GetFileIndex() const noexcept
			{
				return m_FileItem.GetExtraData<Compression::FileIndex>();
			}
			void SetFileIndex(Compression::FileIndex value) noexcept
			{
				m_FileItem.SetExtraData(value);
			}

			wxString GetAttributes() const
			{
				return m_Attributes;
			}
			void SetAttributes(const wxString& string)
			{
				m_Attributes = string;
			}
		
			wxOutputStream* GetOutputStream() const
			{
				return m_OutputStream;
			}
			void SetOutputStream(wxOutputStream* stream)
			{
				m_OutputStream = stream;
			}

			wxInputStream* GetInputStream() const
			{
				return m_InputStream;
			}
			void SetInputStream(wxInputStream* stream)
			{
				m_InputStream = stream;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ArchiveEvent);
	};
}
