#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxSplashWindow.h"
#include "KxFramework/KxTimer.h"
#include <wx/animate.h>

class KX_API KxAnimatedSplashWindow: public KxSplashWindow
{
	private:
		std::unique_ptr<wxAnimation> m_Animation;
		KxTimerMethod<KxAnimatedSplashWindow> m_Timer;
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
							   wxAnimation* animation,
							   int timeout = 0,
							   int style = DefaultStyle
		)
		{
			Create(parent, animation, timeout, style);
		}
		KxAnimatedSplashWindow(wxWindow* parent,
							   wxAnimation* animation,
							   const wxSize& size,
							   int timeout = 0,
							   int style = DefaultStyle
		)
		{
			Create(parent, animation, size, timeout, style);
		}
		bool Create(wxWindow* parent,
					wxAnimation* animation,
					int timeout = 0,
					int style = DefaultStyle
		)
		{
			return Create(parent, animation, animation->GetSize(), timeout, style);
		}
		bool Create(wxWindow* parent,
					wxAnimation* animation,
					const wxSize& size,
					int timeout = 0,
					int style = DefaultStyle
		);
		~KxAnimatedSplashWindow();

	public:
		bool Show(bool show = true) override;
		bool Destroy() override;
		void Play();

		const wxAnimation* GetAnimation() const;
		void SetAnimation(wxAnimation* animation, const wxSize& size = wxDefaultSize);
		void ResetAnimation();

		bool IsLooping() const;
		void SetLooping(bool value = true);

		bool IsAnimationLoaded() const;
		bool IsAnimationEnded() const;

	public:
		wxDECLARE_DYNAMIC_CLASS(KxAnimatedSplashWindow);
};
