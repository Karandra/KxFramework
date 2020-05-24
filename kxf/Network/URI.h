#pragma once
#include "Common.h"
#include "Private/URXHelper.h"

namespace kxf
{
	class KX_API URI: public wxURI, public Network::Private::URXHelper<URI, wxURI>
	{
		protected:
			void NormalizeURI()
			{
				Network::Private::NormalizeURI(m_scheme, m_server, m_path, m_query, m_fragment, m_userinfo, m_port);
			}

		public:
			URI() noexcept
				:THelper(this)
			{
			}
			URI(const String& uri)
				:wxURI(Network::Private::NormalizeInputAddress(uri)), THelper(this)
			{
				NormalizeURI();
			}
			URI(const wxString& uri)
				:URI(String(uri))
			{
				NormalizeURI();
			}
			URI(const char* uri)
				:URI(String(uri))
			{
			}
			URI(const wchar_t* uri)
				:URI(String(uri))
			{
			}
			
			URI(const wxURI& other)
				:URI(other.BuildURI())
			{
			}
			URI(const URI& other)
				:wxURI(other), THelper(this)
			{
			}
			URI(URI&& other) noexcept
				:wxURI(std::move(other)), THelper(this)
			{
				other.Clear();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_fields == 0;
			}
			bool Create(const String& uri)
			{
				if (wxURI::Create(Network::Private::NormalizeInputAddress(uri)))
				{
					NormalizeURI();
					return true;
				}
				return false;
			}
			void Clear() noexcept
			{
				wxURI::Clear();
			}

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
			URI& operator=(const String& uri)
			{
				AsBase() = Network::Private::NormalizeInputAddress(uri);
				NormalizeURI();

				return *this;
			}
			URI& operator=(const wxString& uri)
			{
				*this = String(uri);
				return *this;
			}
			URI& operator=(const char* uri)
			{
				*this = String(uri);
				return *this;
			}
			URI& operator=(const wchar_t* uri)
			{
				*this = String(uri);
				return *this;
			}
			
			URI& operator=(const wxURI& other)
			{
				*this = other.BuildURI();
				return *this;
			}
			URI& operator=(const URI& other)
			{
				AsBase() = other;
				return *this;
			}
			URI& operator=(URI&& other) noexcept
			{
				AsBase() = std::move(other);
				other.Clear();

				return *this;
			}
	};
}
