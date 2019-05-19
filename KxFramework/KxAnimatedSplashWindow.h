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
		virtual void DoResetAnimation();
		virtual void DoPlay();

	public:
		KxAnimatedSplashWindow() {}
		KxAnimatedSplashWindow(wxWindow* parent,
							   wxAnimation* animation,
							   int timeout = 0,
							   int style = DefaultStyle
		)
		{
			Create(parent, animation, timeout, style);
		}
		bool Create(wxWindow* parent,
					wxAnimation* animation,
					int timeout = 0,
					int style = DefaultStyle
		);
		virtual ~KxAnimatedSplashWindow();

	public:
		virtual bool Show(bool show = true) override;
		virtual bool Destroy() override;
		void Play();

		const wxAnimation* GetAnimation() const;
		void SetAnimation(wxAnimation* animation);
		void ResetAnimation();

		bool IsLooping() const;
		void SetLooping(bool value = true);

		bool IsAnimationLoaded() const;
		bool IsAnimationEnded() const;

	public:
		wxDECLARE_DYNAMIC_CLASS(KxAnimatedSplashWindow);
};
