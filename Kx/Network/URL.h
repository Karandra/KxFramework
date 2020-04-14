#pragma once
#include "Common.h"
#include "URI.h"
#include "Private/URXHelper.h"
#include <wx/url.h>

namespace KxFramework
{
	class KX_API URL: public wxURL, public Network::Private::URXHelper<URL, wxURL>
	{
		protected:
			bool IsHTTPS() const;
			void OnAssignAddress();
			void NormalizeURI()
			{
				Network::Private::NormalizeURI(m_scheme, m_server, m_path, m_query, m_fragment, m_userinfo, m_port);
			}

		public:
			URL() noexcept
				:THelper(this)
			{
			}
			URL(const String& url)
				:wxURL(Network::Private::NormalizeInputAddress(url)), THelper(this)
			{
				NormalizeURI();
				OnAssignAddress();
			}
			URL(const wxString& url)
				:URL(String(url))
			{
			}
			URL(const char* url)
				:URL(String(url))
			{
			}
			URL(const wchar_t* url)
				:URL(String(url))
			{
			}
			
			URL(const wxURI& other)
				:URL(other.BuildURI())
			{
			}
			URL(const URL& other)
				:wxURL(other), THelper(this)
			{
			}
			URL(URL&& other) noexcept
				:wxURL(std::move(other)), THelper(this)
			{
				other.Clear();
			}

		public:
			bool IsOk() = delete;
			bool IsNull() const noexcept
			{
				return m_fields == 0 || !wxURL::IsOk();
			}
			void Clear() noexcept
			{
				AsBase() = wxURL();
			}

			URLStatus GetError() const noexcept
			{
				return Network::Private::MapURLStatus(wxURL::GetError());
			}
			URLStatus SetURL(const String& url)
			{
				wxURL::SetURL(Network::Private::NormalizeInputAddress(url));
				NormalizeURI();
				OnAssignAddress();

				return GetError();
			}
			bool Create(const String& uri)
			{
				if (wxURI::Create(Network::Private::NormalizeInputAddress(uri)))
				{
					NormalizeURI();
					OnAssignAddress();

					return true;
				}
				return false;
			}

			std::unique_ptr<wxInputStream> GetInputStream();

			using THelper::GetHostType;
			using THelper::Resolve;

			using THelper::BuildURI;
			using THelper::BuildUnescapedURI;

			using THelper::GetFragment;
			using THelper::GetPassword;
			using THelper::GetPath;
			using THelper::GetPort;
			using THelper::GetQuery;
			using THelper::GetScheme;
			using THelper::GetServer;
			using THelper::GetUser;
			using THelper::GetUserInfo;

		public:
			URL& operator=(const String& url)
			{
				SetURL(url);
				return *this;
			}
			URL& operator=(const wxString& url)
			{
				*this = String(url);
				return *this;
			}
			URL& operator=(const char* url)
			{
				*this = String(url);
				return *this;
			}
			URL& operator=(const wchar_t* url)
			{
				*this = String(url);
				return *this;
			}
			
			URL& operator=(const wxURI& other)
			{
				*this = other.BuildURI();
				return *this;
			}
			URL& operator=(const URL& other)
			{
				AsBase() = other;
				return *this;
			}
			URL& operator=(URL&& other)
			{
				AsBase() = std::move(other);
				other.Clear();

				return *this;
			}
	};
}
