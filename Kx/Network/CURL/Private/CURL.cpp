#include "stdafx.h"
#include "CURL.h"
#include "Kx/Network/CURL/Session.h"

#define CURL_STATICLIB 1
#include <curl/curl.h>
#include <curl/urlapi.h>
#include <curl/curlver.h>

namespace
{
	bool g_IsInitialized = false;

	template<bool easy, class THandle, class TOption, class T>
	int DoSetOptionInt(THandle handle, TOption option, T value)
	{
		using ValueT = typename std::remove_cv<T>::type;

		if constexpr (std::is_pointer<ValueT>::value)
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
		else if constexpr (sizeof(ValueT) <= sizeof(long))
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
		else if constexpr (sizeof(ValueT) <= sizeof(curl_off_t))
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
			if constexpr (easy)
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
	int DoDispatchSetOptionInt(KxFramework::CURL::Private::SessionHandleType type, void* handle, int option, T value)
	{
		using namespace KxFramework::CURL::Private;

		switch (type)
		{
			case SessionHandleType::Easy:
			{
				return DoSetOptionInt<true>(reinterpret_cast<CURL*>(handle), static_cast<CURLoption>(option), value);
			}
			case SessionHandleType::Multi:
			{
				return DoSetOptionInt<true>(reinterpret_cast<CURLM*>(handle), static_cast<CURLMoption>(option), value);
			}
		};
		return -1;
	}
}

namespace KxFramework::CURL::Private
{
	int SessionHandle::SetOption(int option, const String& value, size_t* length)
	{
		auto utf8 = value.ToUTF8();
		if (length)
		{
			*length = utf8.length();
		}
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, utf8.data());
	}
	int SessionHandle::SetOption(int option, int32_t value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	int SessionHandle::SetOption(int option, int64_t value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	int SessionHandle::SetOption(int option, size_t value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	int SessionHandle::SetOption(int option, bool value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
	int SessionHandle::SetOption(int option, const void* value) noexcept
	{
		return DoDispatchSetOptionInt(m_Type, m_Handle, option, value);
	}
}

namespace KxFramework::CURL::Private
{
	class InitializationModule final: public wxModule
	{
		public:
			bool OnInit() override
			{
				g_IsInitialized = curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK;
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
	wxIMPLEMENT_DYNAMIC_CLASS(InitializationModule, wxModule);
}
