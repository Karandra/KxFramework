#pragma once
#include "Common.h"
#include "Node.h"
#include "Element.h"
#include "ScriptValue.h"
#include "Events.h"
#include "EventDispatcher.h"
#include "IWindowRenderer.h"
#include "Renderers/FPSCounter.h"
#include "Kx/Network/URI.h"
#include <wx/window.h>

namespace KxFramework::Sciter
{
	class KX_API Host: public wxObject
	{
		friend class Node;
		friend class Element;
		friend class BasicEventDispatcher;

		private:
			wxWindow& m_SciterWindow;
			WindowEventDispatcher m_EventDispatcher;
			std::unique_ptr<IWindowRenderer> m_Renderer;
			std::unordered_map<wxEvtHandler*, std::unique_ptr<BasicEventDispatcher>> m_ElementEventDispatchers;
			FPSCounter m_FrameCounter;

			bool m_EngineCreated = false;
			bool m_AllowSciterHandleMessage = false;

			bool m_Option_ThemeEnabled = true;
			bool m_Option_SmoothScrolling = true;
			FontSmoothing m_Option_FontSmoothing = FontSmoothing::SystemDefault;
			WindowRenderer m_Option_WindowRenderer = WindowRenderer::Default;

			String m_DocumentPath;
			String m_DocumentBasePath;
			bool m_ReloadScheduled = false;

		private:
			void SetDefaultOptions();
			std::pair<int, int> UpdateWindowStyle();

			void OnEngineCreated();
			void OnEngineDestroyed();
			void OnDocumentChanged();
			bool ProcessEvent(wxEvent& event);

			void AttachElementHandler(Element& element);
			void DetachElementHandler(Element& element);
			void AttachElementHandler(Element& element, wxEvtHandler& evtHandler);
			void DetachElementHandler(Element& element, wxEvtHandler& evtHandler);

		protected:
			bool SciterPreHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
			bool SciterPostHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
			void OnInternalIdle();

		public:
			Host(wxWindow& window);
			Host(const Host&) = delete;
			virtual ~Host();

		public:
			bool Create();
			void Update();
			void Reload();
			bool IsEngineCreated()
			{
				return m_EngineCreated;
			}

			const wxWindow& GetWindow() const
			{
				return m_SciterWindow;
			}
			wxWindow& GetWindow()
			{
				return m_SciterWindow;
			}

			wxSize GetBestSize() const;
			wxSize GetDPI() const;
			double GetFPS() const;

			bool IsSystemThemeEnabled() const;
			bool EnableSystemTheme(bool enable = true);
			
			bool IsSmoothScrollingEnabled() const;
			bool EnableSmoothScrolling(bool enable = true);

			FontSmoothing GetFontSommthingMode() const;
			bool SetFontSmoothingMode(FontSmoothing mode);

			bool SetWindowFrame(WindowFrame windowFrame);
			bool SetBlurBehind(WindowBlurBehind blurMode);

			WindowRenderer GetWindowRenderer() const;
			bool SetWindowRenderer(WindowRenderer renderer);

			bool IsTransparentBackgroundSupported(wxString* reason = nullptr) const;
			bool SetFont(const wxFont& font);
			bool SetBackgroundColor(const Color& color);
			bool SetForegroundColor(const Color& color);

			wxLayoutDirection GetLayoutDirection() const;
			void SetLayoutDirection(wxLayoutDirection value);

		public:
			bool LoadHTML(const String& html, const String& basePath = {});
			bool LoadHTML(const String& html, const URI& baseURI);
			bool SetCSS(const String& css);

			bool LoadDocument(const String& localPath);
			bool LoadDocument(const URI& uri);
			void ClearDocument();

			Element GetRootElement() const;
			Element GetFocusedElement() const;
			Element GetElementByUID(ElementUID* id) const;
			Element GetElementFromPoint(const wxPoint& pos) const;

			Element GetHighlightedElement() const;
			void SetHighlightedElement(const Element& node);

			ScriptValue ExecuteScript(const String& script);

		public:
			Host& operator=(const Host&) = delete;
	};
}
