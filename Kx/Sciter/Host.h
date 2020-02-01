#pragma once
#include "Common.h"
#include "Node.h"
#include "Element.h"
#include <KxFramework/KxURI.h>
#include <wx/window.h>

namespace KxSciter
{
	class KX_API Host
	{
		private:
			wxWindow& m_SciterWindow;
			bool m_EngineCreated = false;
			bool m_AllowSciterHandleMessage = false;

			bool m_Option_ThemeEnabled = true;
			bool m_Option_SmoothScrolling = true;
			FontSmoothing m_Option_FontSmoothing = FontSmoothing::SystemDefault;

			wxString m_LatestBasePath;
			bool m_ReloadScheduled = false;

		private:
			int SciterNotify_LoadData(void* context);
			int SciterNotify_DataLoaded(void* context);
			int SciterNotify_AttachBehavior(void* context);
			int SciterNotify_PostedNotification(void* context);
			int SciterNotify_CriticalFailure();
			int SciterNotify_EngineDestroyed();

			void SetDefaultOptions();
			void SetupCallbacks();

			void OnEngineCreated();
			void OnEngineDestroyed();


		protected:
			bool SciterHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
			int SciterHandleNotify(void* context);

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

			bool IsSystemThemeEnabled() const;
			bool EnableSystemTheme(bool enable = true);
			
			bool IsSmoothScrollingEnabled() const;
			bool EnableSmoothScrolling(bool enable = true);

			FontSmoothing GetFontSommthingMode() const;
			bool SetFontSmoothingMode(FontSmoothing mode);

			bool SetWindowFrame(WindowFrame windowFrame);
			bool SetBlurBehind(WindowBlurBehind blurMode);

			bool IsTransparentBackgroundSupported(wxString* reason = nullptr) const;
			bool SetTransparency(uint8_t alpha);

			bool SetFont(const wxFont& font);
			bool SetBackgroundColor(const KxColor& color);
			bool SetForegroundColor(const KxColor& color);

			wxLayoutDirection GetLayoutDirection() const;
			void SetLayoutDirection(wxLayoutDirection value);

		public:
			bool LoadHTML(const wxString& html, const wxString& basePath = {});
			bool LoadHTML(const wxString& html, const KxURI& baseURI);

			bool LoadDocument(const wxString& localPath);
			bool LoadDocument(const KxURI& uri);

			void ClearDocument();

			Element GetRootElement() const;
			Element GetFocusedElement() const;
			Element GetElementByUID(void* id) const;
			Element GetElementFromPoint(const wxPoint& pos) const;

			Element GetHighlightedElement() const;
			void SetHighlightedElement(const Element& node);

			bool ExecuteScript(const wxString& script);

		public:
			Host& operator=(const Host&) = delete;
	};
}
