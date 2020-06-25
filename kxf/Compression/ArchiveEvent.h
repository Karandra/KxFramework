#pragma once
#include "Common.h"
#include "kxf/FileSystem/FileOperationEvent.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/Crypto/SecretValue.h"
class wxInputStream;
class wxOutputStream;

namespace kxf
{
	class KX_API ArchiveEvent: public FileOperationEvent
	{
		public:
			KxEVENT_MEMBER(ArchiveEvent, Process);
			KxEVENT_MEMBER(ArchiveEvent, GetPassword);

			KxEVENT_MEMBER(ArchiveEvent, GetInputStream);
			KxEVENT_MEMBER(ArchiveEvent, GetOutputStream);

		private:
			FileItem m_FileItem;
			SecretValue m_Password;
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
				auto clone = std::make_unique<ArchiveEvent>(GetEventType());
				clone->SetEventObject(GetEventObject());
				clone->m_FileItem = m_FileItem;
				clone->m_InputStream = m_InputStream;
				clone->m_OutputStream = m_OutputStream;

				return clone.release();
			}
			
			const FileItem& GetItem() const noexcept
			{
				return m_FileItem;
			}
			FileItem& GetItem() noexcept
			{
				return m_FileItem;
			}
			void SetItem(FileItem item) noexcept
			{
				m_FileItem = std::move(item);
			}

			size_t GetFileIndex() const noexcept
			{
				if (auto id = m_FileItem.GetUniqueID().ToLocallyUniqueID())
				{
					return static_cast<size_t>(id.ToInt());
				}
				return std::numeric_limits<size_t>::max();
			}
			void SetFileIndex(size_t value) noexcept
			{
				m_FileItem.SetUniqueID(LocallyUniqueID(value));
			}

			const SecretValue& GetPassword() const& noexcept
			{
				return m_Password;
			}
			SecretValue GetPassword() && noexcept
			{
				return std::move(m_Password);
			}
			void SetPassword(SecretValue password) noexcept
			{
				m_Password = std::move(password);
			}

			wxOutputStream* GetOutputStream() const noexcept
			{
				return m_OutputStream;
			}
			void SetOutputStream(wxOutputStream* stream) noexcept
			{
				m_OutputStream = stream;
			}

			wxInputStream* GetInputStream() const noexcept
			{
				return m_InputStream;
			}
			void SetInputStream(wxInputStream* stream) noexcept
			{
				m_InputStream = stream;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ArchiveEvent);
	};
}
