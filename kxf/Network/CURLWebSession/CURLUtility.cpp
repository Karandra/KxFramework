#include "KxfPCH.h"
#include "CURLUtility.h"
#include "kxf/Utility/ScopeGuard.h"

#define CURL_STATICLIB 1
#include <curl/curl.h>
#include <curl/urlapi.h>

namespace
{
	bool g_IsInitialized = false;

	template<bool easy, class THandle, class TOption, class T>
	int DoSetOptionInt(THandle handle, TOption option, T value)
	{
		using ValueT = typename std::remove_cv<T>::type;

		if constexpr(std::is_pointer<ValueT>::value)
		{
			if constexpr (easy)
			{
				return curl_easy_setopt(handle, static_cast<CURLoption>(option), (void*)value);
			}
			else
			{
				return curl_multi_setopt(handle, static_cast<CURLMoption>(option), (void*)value);
			}
		}
		else if constexpr(sizeof(ValueT) <= sizeof(long))
		{
			if constexpr (easy)
			{
				return curl_easy_setopt(handle, static_cast<CURLoption>(option), static_cast<long>(value));
			}
			else
			{
				return curl_multi_setopt(handle, static_cast<CURLMoption>(option), static_cast<long>(value));
			}
		}
		else if constexpr(sizeof(ValueT) <= sizeof(curl_off_t))
		{
			if constexpr (easy)
			{
				return curl_easy_setopt(handle, static_cast<CURLoption>(option), static_cast<curl_off_t>(value));
			}
			else
			{
				return curl_multi_setopt(handle, static_cast<CURLMoption>(option), static_cast<curl_off_t>(value));
			}
		}
		else
		{
			static_assert(false, "cURL::SetOptionInt: Unknown type");
			if constexpr(easy)
			{
				return CURL_LAST;
			}
			else
			{
				return CURLM_LAST;
			}
		}
	}

	template<class T>
	bool DoDispatchSetOptionInt(kxf::CURL::Private::SessionHandleType type, void* handle, int option, T value)
	{
		using namespace kxf::CURL::Private;

		switch (type)
		{
			case SessionHandleType::Easy:
			{
				return DoSetOptionInt<true>(reinterpret_cast<CURL*>(handle), static_cast<CURLoption>(option), value) == CURLE_OK;
			}
			case SessionHandleType::Multi:
			{
				return DoSetOptionInt<false>(reinterpret_cast<CURLM*>(handle), static_cast<CURLMoption>(option), value) == CURLE_OK;
			}
		};
		return false;
	}
}

namespace kxf::CURL::Private
{
	bool SessionHandle::SetOption(int option, const std::string& utf8)
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, utf8.data());
	}
	bool SessionHandle::SetOption(int option, const String& value, size_t* length)
	{
		auto utf8 = value.ToUTF8();
		if (length)
		{
			*length = utf8.length();
		}
		return SetOption(option, utf8);
	}
	bool SessionHandle::SetOption(int option, int32_t value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool SessionHandle::SetOption(int option, uint32_t value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool SessionHandle::SetOption(int option, int64_t value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool SessionHandle::SetOption(int option, uint64_t value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool SessionHandle::SetOption(int option, bool value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	bool SessionHandle::SetOption(int option, const void* value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
}

namespace kxf::CURL::Private
{
	std::string Escape(const SessionHandle& handle, std::string_view source)
	{
		if (auto encoded = ::curl_easy_escape(handle.GetNativeHandle(), source.data(), source.length()))
		{
			Utility::ScopeGuard atExit = [&]()
			{
				::curl_free(encoded);
			};
			return encoded;
		}
		return {};
	}
	std::string Unescape(const SessionHandle& handle, std::string_view source)
	{
		int encodedLength = 0;
		if (auto encoded = ::curl_easy_unescape(handle.GetNativeHandle(), source.data(), source.length(), &encodedLength))
		{
			Utility::ScopeGuard atExit = [&]()
			{
				::curl_free(encoded);
			};
			return {encoded, static_cast<size_t>(encodedLength)};
		}
		return {};
	}
}

namespace kxf::CURL::Private
{
	class InitializationModule final: public wxModule
	{
		public:
			bool OnInit() override
			{
				g_IsInitialized = ::curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK;
				return g_IsInitialized;
			}
			void OnExit() override
			{
				curl_global_cleanup();
				g_IsInitialized = false;
			}

		private:
			wxDECLARE_DYNAMIC_CLASS(InitializationModule);
	};
}

wxIMPLEMENT_DYNAMIC_CLASS(kxf::CURL::Private::InitializationModule, wxModule);
