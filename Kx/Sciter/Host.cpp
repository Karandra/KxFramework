#include "KxStdAfx.h"
#include "Host.h"
#include "SciterAPI.h"
#include "Internal.h"
#include "KxFramework/KxUtility.h"
#include <WindowsX.h>
#include "Kx/System/UndefWindows.h"

#pragma warning(disable: 4302) // 'reinterpret_cast': truncation from 'void *' to 'UINT'
#pragma warning(disable: 4311) // 'reinterpret_cast': pointer truncation from 'void *' to 'UINT'

namespace KxFramework::Sciter
{
	template<class TEvent>
	TEvent MakeEvent(Host& host, EventID eventID = wxEVT_NULL)
	{
		TEvent event(host);
		event.Allow();
		event.SetEventObject(&host.GetWindow());
		event.SetEventType(eventID);

		return event;
	}
}

namespace KxFramework::Sciter
{
	void Host::SetDefaultOptions()
	{
		EnableSystemTheme();
		EnableSmoothScrolling();
		SetFontSmoothingMode(FontSmoothing::SystemDefault);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_HTTPS_ERROR, 1);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_CONNECTION_TIMEOUT, 20);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_TRANSPARENT_WINDOW, true);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_FILE_IO|ALLOW_SOCKET_IO|ALLOW_EVAL|ALLOW_SYSINFO);
	}
	std::pair<int, int> Host::UpdateWindowStyle()
	{
		const long style = m_SciterWindow.GetWindowStyle();
		const long exStyle = m_SciterWindow.GetExtraStyle();

		// Add 'wxWS_EX_PROCESS_IDLE'. We always need to process idle, especially when using non-default renderer
		// because we're drawing the window content when idle.
		if (!(exStyle & wxWS_EX_PROCESS_IDLE))
		{
			m_SciterWindow.SetExtraStyle(exStyle|wxWS_EX_PROCESS_IDLE);
		}

		return {style, exStyle};
	}

	void Host::OnEngineCreated()
	{
		// Window options
		::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		m_SciterWindow.SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);

		// Create renderer
		m_Renderer = IWindowRenderer::CreateInstance(m_Option_WindowRenderer, *this);
		if (m_Renderer)
		{
			m_Renderer->Create();
		}

		// Sciter options
		SetDefaultOptions();
		m_EventDispatcher.AttachHost();

		// Send event
		Event event = MakeEvent<Event>(*this, EvtEngineCreated);
		ProcessEvent(event);
	}
	void Host::OnEngineDestroyed()
	{
		::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, 0);

		Event event = MakeEvent<Event>(*this, EvtEngineDestroyed);
		ProcessEvent(event);

		m_EventDispatcher.DetachHost();
	}
	void Host::OnDocumentChanged()
	{
		m_SciterWindow.InvalidateBestSize();
	}
	bool Host::ProcessEvent(wxEvent& event)
	{
		return m_SciterWindow.ProcessWindowEvent(event) && !event.GetSkipped();
	}

	void Host::AttachElementHandler(Element& element)
	{
		m_EventDispatcher.AttachElement(element);
	}
	void Host::DetachElementHandler(Element& element)
	{
		m_EventDispatcher.DetachElement(element);
	}
	void Host::AttachElementHandler(Element& element, wxEvtHandler& evtHandler)
	{
		auto [it, newItem] = m_ElementEventDispatchers.insert_or_assign(&evtHandler, std::make_unique<EventDispatcher>(*this, evtHandler));
		it->second->AttachElement(element);
	}
	void Host::DetachElementHandler(Element& element, wxEvtHandler& evtHandler)
	{
		if (auto it = m_ElementEventDispatchers.find(&evtHandler); it != m_ElementEventDispatchers.end())
		{
			it->second->DetachElement(element);
			m_ElementEventDispatchers.erase(it);
		}
	}

	bool Host::SciterPreHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if (m_AllowSciterHandleMessage)
		{
			bool allowSciter = true;
			bool messageHandled = false;
			if (m_Option_WindowRenderer != WindowRenderer::Default && (msg == WM_CREATE || msg == WM_PAINT))
			{
				allowSciter = false;
			}
			if (msg == WM_PAINT && m_SciterWindow.IsFrozen())
			{
				allowSciter = false;
			}

			// Forward messages to Sciter
			if (allowSciter)
			{
				std::optional<FPSCounter::Watcher> fpsWatcher;
				if (msg == WM_PAINT)
				{
					fpsWatcher.emplace(m_FrameCounter.CreateWatcher());
				}

				BOOL handled = FALSE;
				*result = GetSciterAPI()->SciterProcND(m_SciterWindow.GetHandle(), msg, wParam, lParam, &handled);
				messageHandled = handled;
			}

			// Handle engine creation and renderer callbacks
			switch (msg)
			{
				case WM_CREATE:
				{
					m_EngineCreated = true;
					OnEngineCreated();
					break;
				}
				case WM_SIZE:
				{
					if (m_Renderer)
					{
						m_Renderer->OnSize();
					}
					break;
				}
				case WM_PAINT:
				{
					if (m_Renderer)
					{
						m_Renderer->Render();
					}
					break;
				}
				case WM_DESTROY:
				{
					OnEngineDestroyed();
					m_EngineCreated = false;
					m_AllowSciterHandleMessage = false;
					break;
				}
			};
			return messageHandled;
		}
		return false;
	}
	bool Host::SciterPostHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		return false;
	}
	void Host::OnInternalIdle()
	{
		if (m_ReloadScheduled)
		{
			if (auto root = GetRootElement())
			{
				String path = m_DocumentPath;
				LoadHTML(root.GetOuterHTML(), m_DocumentBasePath);
				m_DocumentPath = std::move(path);
			}
			m_ReloadScheduled = false;
		}

		if (m_Renderer)
		{
			m_Renderer->OnIdle();
		}
	}

	Host::Host(wxWindow& window)
		:m_SciterWindow(window), m_EventDispatcher(*this, window)
	{
		if (m_SciterWindow.GetHandle())
		{
			::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		}

		// Child windows are fine with already created window. They don't need 'WS_EX_NOREDIRECTIONBITMAP' style
		if (!m_SciterWindow.IsTopLevel())
		{
			UpdateWindowStyle();
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
			auto [style, exStyle] = UpdateWindowStyle();

			WXDWORD nativeExStyle = 0;
			WXDWORD nativeStyle = m_SciterWindow.MSWGetStyle(style, &nativeExStyle);
			const wxChar* nativeClassName = m_SciterWindow.GetMSWClassName(style);

			const wxPoint pos = m_SciterWindow.GetPosition();
			const wxSize size = m_SciterWindow.GetSize();
			const String title = m_SciterWindow.GetLabel();

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
		return true;
	}
	void Host::Update()
	{
		GetSciterAPI()->SciterUpdateWindow(m_SciterWindow.GetHandle());
		if (m_Renderer)
		{
			m_Renderer->Render();
		}
	}
	void Host::Reload()
	{
		m_ReloadScheduled = true;
	}

	wxSize Host::GetBestSize() const
	{
		const int paddingX = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X, &m_SciterWindow);
		const int paddingY = wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y, &m_SciterWindow);

		int width = GetSciterAPI()->SciterGetMinWidth(m_SciterWindow.GetHandle()) + paddingX;
		int height = GetSciterAPI()->SciterGetMinHeight(m_SciterWindow.GetHandle(), width) + paddingY;

		return m_SciterWindow.FromDIP(wxSize(width, height));
	}
	wxSize Host::GetDPI() const
	{
		UINT x = 0;
		UINT y = 0;
		GetSciterAPI()->SciterGetPPI(m_SciterWindow.GetHandle(), &x, &y);

		return wxSize(x, y);
	}
	double Host::GetFPS() const
	{
		return m_Renderer ? m_Renderer->GetFPS() : m_FrameCounter.GetCount();
	}

	bool Host::IsSystemThemeEnabled() const
	{
		return m_Option_ThemeEnabled;
	}
	bool Host::EnableSystemTheme(bool enable)
	{
		m_Option_ThemeEnabled = enable;
		return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_UX_THEMING, enable);
	}

	bool Host::IsSmoothScrollingEnabled() const
	{
		return m_Option_SmoothScrolling;
	}
	bool Host::EnableSmoothScrolling(bool enable)
	{
		m_Option_SmoothScrolling = enable;
		return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SMOOTH_SCROLL, enable);
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
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 1);
			}
			case FontSmoothing::SystemDefault:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 0);
			}
			case FontSmoothing::Standard:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 2);
			}
			case FontSmoothing::ClearType:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 3);
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

		auto SetAttribute = [&](const String& value)
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

		auto SetAttribute = [&](const String& value)
		{
			GetRootElement().SetAttribute(wxS("window-blurbehind"), value);

			if (!value.IsEmpty())
			{
				return !ExecuteScript(KxString::Format(wxS("view.windowBlurbehind = #%1;"), value)).IsNone();
			}
			return !ExecuteScript(wxS("view.windowBlurbehind = undefined;")).IsNone();
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

	WindowRenderer Host::GetWindowRenderer() const
	{
		return m_Option_WindowRenderer;
	}
	bool Host::SetWindowRenderer(WindowRenderer renderer)
	{
		m_Option_WindowRenderer = renderer;
		return !m_EngineCreated;
	}

	bool Host::IsTransparentBackgroundSupported(wxString* reason) const
	{
		if (m_SciterWindow.IsTopLevel())
		{
			KxUtility::SetIfNotNull(reason, wxS("Always supported for top-level windows"));
			return true;
		}
		else if (m_Renderer)
		{
			KxUtility::SetIfNotNull(reason, wxS("Graphics renderer can enable transparency even for child windows"));
			return true;
		}
		else
		{
			KxUtility::SetIfNotNull(reason, wxS("Not a top-level window and doesn't use a graphics renderer"));
			return false;
		}
	}
	bool Host::SetFont(const wxFont& font)
	{
		return GetRootElement().SetStyleFont(font);
	}
	bool Host::SetBackgroundColor(const Color& color)
	{
		return GetRootElement().SetStyleAttribute(wxS("background-color"), color);
	}
	bool Host::SetForegroundColor(const Color& color)
	{
		return GetRootElement().SetStyleAttribute(wxS("color"), color);
	}

	wxLayoutDirection Host::GetLayoutDirection() const
	{
		String value = GetRootElement().GetStyleAttribute(wxS("direction"));
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

	bool Host::LoadHTML(const String& html, const String& basePath)
	{
		m_DocumentPath.clear();
		m_DocumentBasePath = basePath;
		if (!m_DocumentBasePath.IsEmpty() && !m_DocumentBasePath.StartsWith(wxS("file://")))
		{
			m_DocumentBasePath.Prepend(wxS("file://"));
		}
		if (!m_DocumentBasePath.IsEmpty() && m_DocumentBasePath.back() != wxS('\\'))
		{
			m_DocumentBasePath += wxS('\\');
		}

		auto utf8 = ToSciterUTF8(html);
		return GetSciterAPI()->SciterLoadHtml(m_SciterWindow.GetHandle(), utf8.data(), utf8.size(), m_DocumentBasePath.wc_str());
	}
	bool Host::LoadHTML(const String& html, const KxURI& baseURI)
	{
		m_DocumentPath.clear();
		m_DocumentBasePath = baseURI.BuildURI();
		
		auto utf8 = ToSciterUTF8(html);
		return GetSciterAPI()->SciterLoadHtml(m_SciterWindow.GetHandle(), utf8.data(), utf8.size(), m_DocumentBasePath.wc_str());
	}
	bool Host::SetCSS(const String& css)
	{
		auto utf8 = ToSciterUTF8(css);
		return GetSciterAPI()->SciterSetCSS(m_SciterWindow.GetHandle(), utf8.data(), utf8.size(), m_DocumentBasePath.wc_str(), nullptr);
	}

	bool Host::LoadDocument(const String& localPath)
	{
		m_DocumentPath = localPath;
		m_DocumentBasePath = localPath.BeforeLast(wxS('\\'));
		return GetSciterAPI()->SciterLoadFile(m_SciterWindow.GetHandle(), m_DocumentPath.wc_str());
	}
	bool Host::LoadDocument(const KxURI& uri)
	{
		m_DocumentPath = uri.BuildURI();
		m_DocumentBasePath = m_DocumentPath;
		return GetSciterAPI()->SciterLoadFile(m_SciterWindow.GetHandle(), m_DocumentBasePath.wc_str());
	}
	void Host::ClearDocument()
	{
		m_DocumentBasePath.clear();
		LoadHTML(wxEmptyString);
	}

	Element Host::GetRootElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetRootElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetFocusedElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetFocusElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetElementByUID(ElementUID* id) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetElementByUID(m_SciterWindow.GetHandle(), reinterpret_cast<UINT>(id), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetElementFromPoint(const wxPoint& pos) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterFindElement(m_SciterWindow.GetHandle(), {pos.x, pos.y}, &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}

	Element Host::GetHighlightedElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetHighlightedElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	void Host::SetHighlightedElement(const Element& node)
	{
		GetSciterAPI()->SciterSetHighlightedElement(m_SciterWindow.GetHandle(), (HELEMENT)node.GetHandle());
	}

	ScriptValue Host::ExecuteScript(const String& script)
	{
		ScriptValue result;
		GetSciterAPI()->SciterEval(m_SciterWindow.GetHandle(), script.wc_str(), script.length(), reinterpret_cast<SCITER_VALUE*>(&result.GetNativeValue()));
		return result;
	}
}
