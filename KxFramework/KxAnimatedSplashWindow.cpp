#include "KxStdAfx.h"
#include "KxFramework/KxAnimatedSplashWindow.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxAnimatedSplashWindow, KxSplashWindow)

//////////////////////////////////////////////////////////////////////////
void KxAnimatedSplashWindow::OnTimer(wxTimerEvent& event)
{
	// Update frame counter
	m_CurrentFrame++;
	if (m_CurrentFrame > m_Animation->GetFrameCount())
	{
		m_CurrentFrame = 0;
	}

	// Update view
	DoSetSplash(wxBitmap(m_Animation->GetFrame(m_CurrentFrame), 32), wxDefaultSize);
	DoUpdateSplash();

	// Schedule next frame
	if (!m_ShouldStop && (!IsAnimationEnded() || m_ShouldLoop))
	{
		m_Timer.StartOnce(m_Animation->GetDelay(m_CurrentFrame));
	}
}
void KxAnimatedSplashWindow::OnDestroy()
{
	m_ShouldStop = true;
	m_Timer.Stop();
}

void KxAnimatedSplashWindow::DoResetAnimation(const wxSize& size)
{
	m_Timer.Stop();
	m_CurrentFrame = 0;

	DoSetSplash(wxBitmap(m_Animation->GetFrame(0), 32), size.IsFullySpecified() ? size : GetClientSize());
}
void KxAnimatedSplashWindow::DoPlay()
{
	m_Timer.StartOnce(m_Animation->GetDelay(0));
}

bool KxAnimatedSplashWindow::Create(wxWindow* parent,
									std::unique_ptr<wxAnimation> animation,
									const wxSize& size,
									int timeout,
									int style
)
{
	m_Animation = std::move(animation);

	if (KxSplashWindow::Create(parent, m_Animation->GetFrame(0), size, timeout, style))
	{
		m_Timer.BindFunction(&KxAnimatedSplashWindow::OnTimer, this);
		return true;
	}
	return false;
}
KxAnimatedSplashWindow::~KxAnimatedSplashWindow()
{
	OnDestroy();
}

void KxAnimatedSplashWindow::SetAnimation(std::unique_ptr<wxAnimation> animation, const wxSize& size)
{
	m_Animation = std::move(animation);
	DoResetAnimation(size);
	ScheduleRefresh();
}
void KxAnimatedSplashWindow::ResetAnimation()
{
	DoResetAnimation(wxDefaultSize);
	ScheduleRefresh();
}

bool KxAnimatedSplashWindow::IsLooping() const
{
	return m_ShouldLoop;
}
void KxAnimatedSplashWindow::SetLooping(bool value)
{
	m_ShouldLoop = value;
	if (IsAnimationEnded())
	{
		DoPlay();
		ScheduleRefresh();
	}
}

bool KxAnimatedSplashWindow::IsAnimationLoaded() const
{
	return m_Animation->GetFrameCount() != 0;
}
bool KxAnimatedSplashWindow::IsAnimationEnded() const
{
	return m_Animation->GetFrameCount() != 0 && m_CurrentFrame >= m_Animation->GetFrameCount();
}

bool KxAnimatedSplashWindow::Show(bool show)
{
	DoResetAnimation(wxDefaultSize);
	DoPlay();
	ScheduleRefresh();

	return KxSplashWindow::Show(show);
}
bool KxAnimatedSplashWindow::Destroy()
{
	OnDestroy();
	return KxSplashWindow::Destroy();
}
void KxAnimatedSplashWindow::Play()
{
	DoResetAnimation(wxDefaultSize);
	DoPlay();
	ScheduleRefresh();
}
