#include "KxStdAfx.h"
#include "Host.h"
#include "Kx/Sciter/SciterAPI/sciter-x.h"
#include "Kx/Sciter/SciterAPI/sciter-x-api.h"

namespace KxSciter
{
	int Host::SciterNotify_LoadData(void* context)
	{
		SCN_LOAD_DATA& notification = *reinterpret_cast<SCN_LOAD_DATA*>(context);

		LPCBYTE pb = nullptr;
		UINT cb = 0;
		aux::wchars wu = aux::chars_of(notification.uri);

		if (wu.like(WSTR("res:*")))
		{
			if (sciter::load_resource_data(nullptr, wu.start + 4, pb, cb))
			{
				::SciterDataReady(m_SciterWindow.GetHandle(), notification.uri, pb, cb);
			}
			return LOAD_DISCARD;
		}
		else if (wu.like(WSTR("this://app/*")))
		{
			// try to get them from archive first
			aux::bytes adata = sciter::archive::instance().get(wu.start+11);
			if (adata.length)
			{
				::SciterDataReady(m_SciterWindow.GetHandle(), notification.uri, adata.start, adata.length);
			}
			return LOAD_DISCARD;
		}
		return LOAD_OK;
	}
	int Host::SciterNotify_DataLoaded(void* context)
	{
		SCN_DATA_LOADED& notification = *reinterpret_cast<SCN_DATA_LOADED*>(context);
		return 0;
	}
	int Host::SciterNotify_AttachBehavior(void* context)
	{
		SCN_ATTACH_BEHAVIOR& notification = *reinterpret_cast<SCN_ATTACH_BEHAVIOR*>(context);
		return sciter::create_behavior(&notification);
	}
	int Host::SciterNotify_PostedNotification(void* context)
	{
		SCN_POSTED_NOTIFICATION& notification = *reinterpret_cast<SCN_POSTED_NOTIFICATION*>(context);
		return 0;
	}
	int Host::SciterNotify_CriticalFailure()
	{
		return 0;
	}
	int Host::SciterNotify_EngineDestroyed()
	{
		return 0;
	}

	void Host::SetDefaultOptions()
	{
		EnableSystemTheme();
		EnableSmoothScrolling();
		SetFontSmoothingMode(FontSmoothing::SystemDefault);
		::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_TRANSPARENT_WINDOW, true);
		::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_CONNECTION_TIMEOUT, 500);
		::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_HTTPS_ERROR, 1);
	}
	void Host::SetupCallbacks()
	{
		::SciterSetCallback(m_SciterWindow.GetHandle(), [](SCITER_CALLBACK_NOTIFICATION* notification, void* context) -> UINT
		{
			if (notification && context)
			{
				return reinterpret_cast<Host*>(context)->SciterHandleNotify(notification);
			}
			return 0;
		}, this);
	}

	bool Host::SciterHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if (m_AllowSciterHandleMessage)
		{
			// Forward message to Sciter
			BOOL handled = FALSE;
			*result = ::SciterProcND(m_SciterWindow.GetHandle(), msg, wParam, lParam, &handled);

			// Handle engine creation callbacks
			if (msg == WM_CREATE)
			{
				m_EngineCreated = true;

				SetDefaultOptions();
				SetupCallbacks();
			}
			else if (msg == WM_DESTROY)
			{
				m_EngineCreated = false;
				m_AllowSciterHandleMessage = false;
			}
			return handled;
		}
		return false;
	}
	int Host::SciterHandleNotify(void* context)
	{
		SCITER_CALLBACK_NOTIFICATION& notification = *reinterpret_cast<SCITER_CALLBACK_NOTIFICATION*>(context);

		switch (notification.code)
		{
			case SC_LOAD_DATA:
			{
				return SciterNotify_LoadData(context);
			}
			case SC_DATA_LOADED:
			{
				return SciterNotify_DataLoaded(context);
			}
			case SC_ATTACH_BEHAVIOR:
			{
				return SciterNotify_AttachBehavior(context);
			}
			case SC_POSTED_NOTIFICATION:
			{
				return SciterNotify_PostedNotification(context);
			}
			case SC_GRAPHICS_CRITICAL_FAILURE:
			{
				return SciterNotify_CriticalFailure();
			}
			case SC_ENGINE_DESTROYED:
			{
				return SciterNotify_EngineDestroyed();
			}
		};
		return 0;
	}

	Host::Host(wxWindow& window)
		:m_SciterWindow(window)
	{
		// Child windows are fine with already created window. They don't need 'WS_EX_NOREDIRECTIONBITMAP' style
		if (!m_SciterWindow.IsTopLevel())
		{
			m_AllowSciterHandleMessage = true;
		}
	}
	Host::~Host()
	{
	}

	bool Host::Create()
	{
		if (!m_EngineCreated && m_SciterWindow.IsTopLevel())
		{
			// Get original window info
			const long style = m_SciterWindow.GetWindowStyle();
			const wxChar* nativeClassName = m_SciterWindow.GetMSWClassName(style);

			WXDWORD nativeExStyle = 0;
			WXDWORD nativeStyle = m_SciterWindow.MSWGetStyle(style, &nativeExStyle);

			const wxPoint pos = m_SciterWindow.GetPosition();
			const wxSize size = m_SciterWindow.GetSize();
			const wxString title = m_SciterWindow.GetLabel();

			// Destroy and detach original window
			::DestroyWindow(m_SciterWindow.GetHandle());
			m_SciterWindow.DissociateHandle();

			// Create new window with 'WS_EX_NOREDIRECTIONBITMAP' extended style instead and attach it to the wxWindow
			m_AllowSciterHandleMessage = true;
			m_SciterWindow.MSWCreate(nativeClassName, title.wc_str(), pos, size, nativeStyle, nativeExStyle|WS_EX_NOREDIRECTIONBITMAP);

			return m_EngineCreated;
		}
		return false;
	}
	void Host::Update()
	{
		::SciterUpdateWindow(m_SciterWindow.GetHandle());
	}

	wxSize Host::GetBestSize() const
	{
		const int paddingX = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X, &m_SciterWindow);
		const int paddingY = wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y, &m_SciterWindow);

		int width = ::SciterGetMinWidth(m_SciterWindow.GetHandle()) + paddingX;
		int height = ::SciterGetMinHeight(m_SciterWindow.GetHandle(), width) + paddingY;

		return m_SciterWindow.FromDIP(wxSize(width, height));
	}
	wxSize Host::GetDPI() const
	{
		UINT x = 0;
		UINT y = 0;
		::SciterGetPPI(m_SciterWindow.GetHandle(), &x, &y);

		return wxSize(x, y);
	}

	bool Host::IsSystemThemeEnabled() const
	{
		return m_Option_ThemeEnabled;
	}
	bool Host::EnableSystemTheme(bool enable)
	{
		m_Option_ThemeEnabled = enable;
		return ::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_UX_THEMING, enable);
	}

	bool Host::IsSmoothScrollingEnabled() const
	{
		return m_Option_SmoothScrolling;
	}
	bool Host::EnableSmoothScrolling(bool enable)
	{
		m_Option_SmoothScrolling = enable;
		return ::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SMOOTH_SCROLL, enable);
	}

	FontSmoothing Host::GetFontSommthingMode() const
	{
		return m_Option_FontSmoothing;
	}
	bool Host::SetFontSmoothingMode(FontSmoothing mode)
	{
		switch (mode)
		{
			case FontSmoothing::None:
			{
				return ::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 1);
			}
			case FontSmoothing::SystemDefault:
			{
				return ::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 0);
			}
			case FontSmoothing::Standard:
			{
				return ::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 2);
			}
			case FontSmoothing::ClearType:
			{
				return ::SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 3);
			}
		};
		return false;
	}

	bool Host::IsTransparentBackgroundSupported(wxString* reason) const
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			if (reason)
			{
				*reason = "Not a top-level window";
			}
			return false;
		}
		return true;
	}
	bool Host::SetTransparency(uint8_t alpha)
	{
		return false;
	}

	bool Host::LoadHTML(const wxString& html, const wxString& basePath)
	{
		wxString basePathCopy = basePath;
		if (!basePathCopy.IsEmpty() && !basePathCopy.StartsWith(wxS("file://")))
		{
			basePathCopy.Prepend(wxS("file://"));
		}
		if (!basePathCopy.IsEmpty() && basePathCopy.Last() != wxS('\\'))
		{
			basePathCopy += wxS('\\');
		}

		auto utf8 = html.ToUTF8();
		return ::SciterLoadHtml(m_SciterWindow.GetHandle(), reinterpret_cast<const BYTE*>(utf8.data()), utf8.length(), basePathCopy.wc_str());
	}
	bool Host::LoadHTML(const wxString& html, const KxURI& baseURI)
	{
		auto utf8 = html.ToUTF8();
		wxString basePath = baseURI.BuildURI();

		return ::SciterLoadHtml(m_SciterWindow.GetHandle(), reinterpret_cast<const BYTE*>(utf8.data()), utf8.length(), basePath.wc_str());
	}

	bool Host::LoadDocument(const wxString& localPath)
	{
		return ::SciterLoadFile(m_SciterWindow.GetHandle(), localPath.wc_str());
	}
	bool Host::LoadDocument(const KxURI& uri)
	{
		return LoadDocument(uri.BuildURI());
	}

	bool Host::ClearDocument()
	{
		return LoadHTML(wxEmptyString);
	}

	Element Host::GetRootElement() const
	{
		HELEMENT node = nullptr;
		if (::SciterGetRootElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return Element(node);
		}
		return {};
	}
}
