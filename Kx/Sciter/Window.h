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

			bool m_Option_ThemeEnabled = true;
			bool m_Option_SmoothScrolling = true;
			FontSmoothing m_Option_FontSmoothing = FontSmoothing::SystemDefault;

		private:
			int SciterNotify_LoadData(void* context);
			int SciterNotify_DataLoaded(void* context);
			int SciterNotify_AttachBehavior(void* context);
			int SciterNotify_PostedNotification(void* context);
			int SciterNotify_CriticalFailure();
			int SciterNotify_EngineDestroyed();

			void SetDefaultOptions();
			void SetupCallbacks();

		protected:
			bool SciterHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam);
			int SciterHandleNotify(void* context);

		public:
			Host(wxWindow& window)
				:m_SciterWindow(window)
			{
			}
			Host(const Host&) = delete;
			virtual ~Host();

		public:
			bool Create();
			void Update();

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

			bool IsTransparentBackgroundSupported(wxString* reason = nullptr) const;
			bool SetTransparency(uint8_t alpha);

		public:
			bool LoadHTML(const wxString& html, const wxString& basePath = {});
			bool LoadHTML(const wxString& html, const KxURI& baseURI);

			bool LoadDocument(const wxString& localPath);
			bool LoadDocument(const KxURI& uri);

			bool ClearDocument();

			Element GetRootElement() const;

		public:
			Host& operator=(const Host&) = delete;
	};
}

namespace KxSciter
{
	template<class TWindow>
	class Window: public TWindow, public Host
	{
		protected:
			bool MSWHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override
			{
				if (SciterHandleMessage(result, msg, wParam, lParam))
				{
					return true;
				}
				return TWindow::MSWHandleMessage(result, msg, wParam, lParam);
			}
			
			wxSize DoGetBestSize() const override
			{
				return Host::GetBestSize();
			}

		public:
			Window()
				:Host(static_cast<wxWindow&>(*this))
			{
			}
			Window(const Window&) = delete;

			template<class... Args>
			Window(Args&&... arg)
				:TWindow(std::forward<Args>(arg)...), Host(static_cast<wxWindow&>(*this))
			{
				Host::Create();
			}

		public:
			template<class... Args>
			bool Create(Args&&... arg)
			{
				return TWindow::Create(std::forward<Args>(arg)...) && Host::Create();
			}
			
			void Update() override
			{
				Host::Update();
			}
			void Refresh(bool eraseBackground = true, const wxRect* rect = nullptr) override
			{
				Host::Update();
			}

			bool IsSystemThemeEnabled() const
			{
				return Host::IsSystemThemeEnabled();
			}
			void EnableSystemTheme(bool enable = true)
			{
				Host::EnableSystemTheme(enable);
			}

			bool GetThemeEnabled() const override
			{
				return Host::IsSystemThemeEnabled();
			}
			void SetThemeEnabled(bool enable = true)
			{
				TWindow::SetThemeEnabled(true);
			}
			
			bool IsTransparentBackgroundSupported(wxString* reason = nullptr) const
			{
				return Host::IsTransparentBackgroundSupported(reason);;
			}
			bool HasTransparentBackground() override
			{
				return true;
			}
			bool CanSetTransparent() override
			{
				return true;
			}
			bool SetTransparent(wxByte alpha)
			{
				return Host::SetTransparency(alpha);
			}
			
			wxSize GetDPI() const override
			{
				return Host::GetDPI();
			}
	
		public:
			Window& operator=(const Window&) = delete;
	};
}
