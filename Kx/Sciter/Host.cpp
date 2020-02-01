#include "KxStdAfx.h"
#include "Host.h"
#include "Kx/Sciter/SciterAPI/sciter-x.h"
#include "Kx/Sciter/SciterAPI/sciter-x-api.h"

#pragma warning(disable: 4302) // 'reinterpret_cast': truncation from 'void *' to 'UINT'
#pragma warning(disable: 4311) // 'reinterpret_cast': pointer truncation from 'void *' to 'UINT'

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

	void Host::OnEngineCreated()
	{
		// Sciter options
		SetDefaultOptions();
		SetupCallbacks();

		// Window options
		m_SciterWindow.SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
	}
	void Host::OnEngineDestroyed()
	{
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
				::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
				m_EngineCreated = true;

				OnEngineCreated();
			}
			else if (msg == WM_DESTROY)
			{
				OnEngineDestroyed();
				m_EngineCreated = false;
				m_AllowSciterHandleMessage = false;

				::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, 0);
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
	void Host::OnInternalIdle()
	{
		if (m_ReloadScheduled)
		{
			if (auto root = GetRootElement())
			{
				LoadHTML(root.GetOuterHTML(), m_LatestBasePath);
			}
			m_ReloadScheduled = false;
		}
	}

	Host::Host(wxWindow& window)
		:m_SciterWindow(window)
	{
		// Child windows are fine with already created window. They don't need 'WS_EX_NOREDIRECTIONBITMAP' style
		if (!m_SciterWindow.IsTopLevel())
		{
			m_AllowSciterHandleMessage = true;
			::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
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
			const HWND oldHandle = m_SciterWindow.GetHandle();
			m_SciterWindow.DestroyChildren();
			m_SciterWindow.DissociateHandle();
			::DestroyWindow(oldHandle);

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
	void Host::Reload()
	{
		m_ReloadScheduled = true;
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

	bool Host::SetWindowFrame(WindowFrame windowFrame)
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			return false;
		}

		auto SetAttribute = [&](const wxString& value)
		{
			if (GetRootElement().SetAttribute(wxS("window-frame"), value))
			{
				Reload();
				return true;
			}
			return false;
		};

		switch (windowFrame)
		{
			case WindowFrame::Standard:
			{
				return SetAttribute(wxS("none"));
			}
			case WindowFrame::Solid:
			{
				return SetAttribute(wxS("solid"));
			}
			case WindowFrame::SolidWithShadow:
			{
				return SetAttribute(wxS("solid-with-shadow"));
			}
			case WindowFrame::Extended:
			{
				return SetAttribute(wxS("extended"));
			}
			case WindowFrame::Transparent:
			{
				return SetAttribute(wxS("transparent"));
			}
		};
		return false;
	}
	bool Host::SetBlurBehind(WindowBlurBehind blurMode)
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			return false;
		}

		auto SetAttribute = [&](const wxString& value)
		{
			GetRootElement().SetAttribute(wxS("window-blurbehind"), value);

			if (!value.IsEmpty())
			{
				return ExecuteScript(KxString::Format(wxS("view.windowBlurbehind = #%1;"), value));
			}
			return ExecuteScript(wxS("view.windowBlurbehind = undefined;"));
		};

		switch (blurMode)
		{
			case WindowBlurBehind::None:
			{
				return SetAttribute(wxS(""));
			}
			case WindowBlurBehind::Auto:
			{
				return SetAttribute(wxS("auto"));
			}
			case WindowBlurBehind::Light:
			{
				return SetAttribute(wxS("light"));
			}
			case WindowBlurBehind::UltraLight:
			{
				return SetAttribute(wxS("ultra-light"));
			}
			case WindowBlurBehind::Dark:
			{
				return SetAttribute(wxS("dark"));
			}
			case WindowBlurBehind::UltraDark:
			{
				return SetAttribute(wxS("ultra-dark"));
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

	bool Host::SetFont(const wxFont& font)
	{
		return GetRootElement().SetStyleFont(font);
	}
	bool Host::SetBackgroundColor(const KxColor& color)
	{
		return GetRootElement().SetStyleAttribute(wxS("background-color"), color);
	}
	bool Host::SetForegroundColor(const KxColor& color)
	{
		return GetRootElement().SetStyleAttribute(wxS("color"), color);
	}

	wxLayoutDirection Host::GetLayoutDirection() const
	{
		wxString value = GetRootElement().GetStyleAttribute(wxS("direction"));
		if (value == wxS("ltr"))
		{
			return wxLayoutDirection::wxLayout_LeftToRight;
		}
		else if (value == wxS("rtl"))
		{
			return wxLayoutDirection::wxLayout_RightToLeft;
		}
		return wxLayoutDirection::wxLayout_Default;
	}
	void Host::SetLayoutDirection(wxLayoutDirection value)
	{
		switch (value)
		{
			case wxLayoutDirection::wxLayout_LeftToRight:
			{
				GetRootElement().RemoveStyleAttribute(wxS("ltr"));
				break;
			}
			case wxLayoutDirection::wxLayout_RightToLeft:
			{
				GetRootElement().RemoveStyleAttribute(wxS("rtl"));
				break;
			}
			default:
			{
				GetRootElement().RemoveStyleAttribute(wxS("direction"));
			}
		};
	}

	bool Host::LoadHTML(const wxString& html, const wxString& basePath)
	{
		m_LatestBasePath = basePath;
		if (!m_LatestBasePath.IsEmpty() && !m_LatestBasePath.StartsWith(wxS("file://")))
		{
			m_LatestBasePath.Prepend(wxS("file://"));
		}
		if (!m_LatestBasePath.IsEmpty() && m_LatestBasePath.Last() != wxS('\\'))
		{
			m_LatestBasePath += wxS('\\');
		}

		auto utf8 = html.ToUTF8();
		return ::SciterLoadHtml(m_SciterWindow.GetHandle(), reinterpret_cast<const BYTE*>(utf8.data()), utf8.length(), m_LatestBasePath.wc_str());
	}
	bool Host::LoadHTML(const wxString& html, const KxURI& baseURI)
	{
		auto utf8 = html.ToUTF8();
		m_LatestBasePath = baseURI.BuildURI();

		return ::SciterLoadHtml(m_SciterWindow.GetHandle(), reinterpret_cast<const BYTE*>(utf8.data()), utf8.length(), m_LatestBasePath.wc_str());
	}

	bool Host::LoadDocument(const wxString& localPath)
	{
		m_LatestBasePath = localPath.AfterLast(wxS('\\'));
		return ::SciterLoadFile(m_SciterWindow.GetHandle(), localPath.wc_str());
	}
	bool Host::LoadDocument(const KxURI& uri)
	{
		m_LatestBasePath = uri.BuildURI();
		return ::SciterLoadFile(m_SciterWindow.GetHandle(), m_LatestBasePath.wc_str());
	}

	void Host::ClearDocument()
	{
		m_LatestBasePath.clear();
		LoadHTML(wxEmptyString);
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
	Element Host::GetFocusedElement() const
	{
		HELEMENT node = nullptr;
		if (::SciterGetFocusElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return Element(node);
		}
		return {};
	}
	Element Host::GetElementByUID(void* id) const
	{
		HELEMENT node = nullptr;
		if (::SciterGetElementByUID(m_SciterWindow.GetHandle(), reinterpret_cast<UINT>(id), &node) == SCDOM_OK)
		{
			return Element(node);
		}
		return {};
	}
	Element Host::GetElementFromPoint(const wxPoint& pos) const
	{
		HELEMENT node = nullptr;
		if (::SciterFindElement(m_SciterWindow.GetHandle(), {pos.x, pos.y}, &node) == SCDOM_OK)
		{
			return Element(node);
		}
		return {};
	}

	Element Host::GetHighlightedElement() const
	{
		HELEMENT node = nullptr;
		if (::SciterGetHighlightedElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return Element(node);
		}
		return {};
	}
	void Host::SetHighlightedElement(const Element& node)
	{
		::SciterSetHighlightedElement(m_SciterWindow.GetHandle(), (HELEMENT)node.GetHandle());
	}

	bool Host::ExecuteScript(const wxString& script)
	{
		SCITER_VALUE returnValue;
		return ::SciterEval(m_SciterWindow.GetHandle(), script.wc_str(), script.length(), &returnValue);
	}
}
