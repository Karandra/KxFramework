#include "stdafx.h"
#include "URL.h"
#include "CURL/Session.h"

namespace kxf::Network::Private
{
	class URLStream final: public wxInputStream
	{
		private:
			CURLSession m_Session;
			std::vector<uint8_t> m_Data;
			bool m_IsDownloaded = false;

			size_t m_LastRead = 0;
			wxFileOffset m_Position = 0;
			wxFileOffset m_ContentLength = 0;

		private:
			void OnDownload(CURLEvent& event)
			{
				m_ContentLength = event.GetProcessed().GetBytes();
			}
			void DownloadData()
			{
				wxMemoryOutputStream stream;
				CURLStreamReply reply(stream);
				m_Session.Download(reply);

				m_Data.resize(m_ContentLength);
				stream.CopyTo(m_Data.data(), m_Data.size());
			}

		protected:
			size_t OnSysRead(void* buffer, size_t size) override
			{
				if (!m_IsDownloaded)
				{
					DownloadData();
					m_IsDownloaded = true;
				}

				m_LastRead = 0;
				if (m_Position < m_ContentLength && m_ContentLength != 0)
				{
					size = std::clamp<size_t>(size, 0, m_ContentLength - m_Position);
					std::memcpy(buffer, m_Data.data() + m_Position, size);
				
					m_Position += size;
					m_LastRead = size;
				}
				return m_LastRead;
			}

		public:
			URLStream(const URL& url)
				:m_Session(url)
			{
				m_Session.Bind(CURLEvent::EvtDownload, &URLStream::OnDownload, this);
			}

		public:
			wxFileOffset TellI() const override
			{
				return m_Position;
			}
			wxFileOffset SeekI(wxFileOffset pos, wxSeekMode mode = wxSeekMode::wxFromStart) override
			{
				switch (mode)
				{
					case wxSeekMode::wxFromStart:
					{
						m_Position = std::clamp<wxFileOffset>(pos, 0, m_ContentLength);
						return m_Position;
					}
					case wxSeekMode::wxFromEnd:
					{
						m_Position = std::clamp<wxFileOffset>(m_ContentLength - pos, 0, m_ContentLength);
						return m_Position;
					}
					case wxSeekMode::wxFromCurrent:
					{
						m_Position = std::clamp<wxFileOffset>(m_Position + pos, 0, m_ContentLength);
						return m_Position;
					}
				};
				return wxInvalidOffset;
			}

			URLStream& Read(void* buffer, size_t size) override
			{
				OnSysRead(buffer, size);
				return *this;
			}
			char Peek() override
			{
				char c = 0;
				if (OnSysRead(&c, sizeof(c)) == sizeof(c))
				{
					m_Position -= 1;
				}
				return c;
			}
			size_t LastRead() const override
			{
				return m_LastRead;
			}
		
			bool Eof() const override
			{
				return m_Position >= m_ContentLength;
			}
			bool CanRead() const override
			{
				return !m_Data.empty();
			}
	};
}

namespace kxf
{
	bool URL::IsHTTPS() const
	{
		return HasScheme() && GetScheme() == wxS("https");
	}
	void URL::OnAssignAddress()
	{
		// wxURL doesn't support https and seems to know nothing about it, so we're going to teach it.
		if (GetError() == URLStatus::NoProtocol && IsHTTPS())
		{
			// Temporarily change scheme to 'http' to allow FetchProtocol do its work
			// by fetching HTTP protocol instance.
			m_scheme = wxS("http");
			const bool protocolFetched = FetchProtocol();
			m_scheme = wxS("https");

			// Change error to success if we got protocol.
			m_error = protocolFetched ? wxURL_NOERR : wxURL_NOPROTO;

			// Created protocol instance won't allow to connect to the website using it (because the site uses HTTPS),
			// but at least allows to return valid pointer from 'wxURL::GetProtocol'. It's important because we're telling
			// the object that it's valid and in this case we are required to return a valid object.
		}
	}

	std::unique_ptr<wxInputStream> URL::GetInputStream()
	{
		if (!IsNull() && IsHTTPS())
		{
			// wxHTTP can't return a stream that is capable to read content from HTTPS site we can use CURL.
			return std::make_unique<Network::Private::URLStream>(*this);
		}
		return std::unique_ptr<wxInputStream>(wxURL::GetInputStream());
	}
}
