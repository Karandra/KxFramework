#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxSplashWindow.h"
#include <wx/animate.h>

class KX_API KxAnimatedSplashWindow: public KxSplashWindow
{
	private:
		std::unique_ptr<wxAnimation> m_Animation;
		wxTimer m_Timer;
		size_t m_CurrentFrame = 0;
		bool m_ShouldLoop = false;
		bool m_ShouldStop = false;

	private:
		void OnTimer(wxTimerEvent& event);
		void OnDestroy();

	protected:
		virtual void DoResetAnimation(const wxSize& size);
		virtual void DoPlay();

	public:
		KxAnimatedSplashWindow() = default;
		KxAnimatedSplashWindow(wxWindow* parent,
							   std::unique_ptr<wxAnimation> animation,
							   wxTimeSpan timeout = {},
							   int style = DefaultStyle
		)
		{
			Create(parent, std::move(animation), timeout, style);
		}
		KxAnimatedSplashWindow(wxWindow* parent,
							   std::unique_ptr<wxAnimation> animation,
							   const wxSize& size,
							   wxTimeSpan timeout = {},
							   int style = DefaultStyle
		)
		{
			Create(parent, std::move(animation), size, timeout, style);
		}
		bool Create(wxWindow* parent,
					std::unique_ptr<wxAnimation> animation,
					wxTimeSpan timeout = {},
					int style = DefaultStyle
		)
		{
			return Create(parent, std::move(animation), animation->GetSize(), timeout, style);
		}
		bool Create(wxWindow* parent,
					std::unique_ptr<wxAnimation> animation,
					const wxSize& size,
					wxTimeSpan timeout = {},
					int style = DefaultStyle
		);
		~KxAnimatedSplashWindow();

	public:
		bool Show(bool show = true) override;
		bool Destroy() override;
		void Play();

		bool HasNaimation() const
		{
			return m_Animation != nullptr;
		}
		const wxAnimation& GetAnimation() const
		{
			return *m_Animation;
		}
		void SetAnimation(std::unique_ptr<wxAnimation> animation, const wxSize& size = wxDefaultSize);
		void ResetAnimation();

		bool IsLooping() const;
		void SetLooping(bool value = true);

		bool IsAnimationLoaded() const;
		bool IsAnimationEnded() const;

	public:
		wxDECLARE_DYNAMIC_CLASS(KxAnimatedSplashWindow);
};
