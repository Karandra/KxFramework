#include "KxStdAfx.h"
#include "KxFramework/KxAnimatedSplashWindow.h"
#include "KxFramework/KxUtility.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxAnimatedSplashWindow, KxSplashWindow)

//////////////////////////////////////////////////////////////////////////
void KxAnimatedSplashWindow::OnTimer()
{
	// Update frame counter
	m_CurrentFrame++;
	if (m_CurrentFrame > m_Animation->GetFrameCount())
	{
		m_CurrentFrame = 0;
	}

	// Update view
	DoSetSplash(wxBitmap(m_Animation->GetFrame(m_CurrentFrame), 32));
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

void KxAnimatedSplashWindow::DoResetAnimation()
{
	m_Timer.Stop();
	m_CurrentFrame = 0;
	DoSetSplash(m_Animation->GetFrame(0));
}
void KxAnimatedSplashWindow::DoPlay()
{
	m_Timer.StartOnce(m_Animation->GetDelay(0));
}

bool KxAnimatedSplashWindow::Create(wxWindow* parent,
									wxAnimation* animation,
									int timeout,
									int style
)
{
	m_Animation.reset(animation);
	if (KxSplashWindow::Create(parent, m_Animation->GetFrame(0), timeout, style))
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

const wxAnimation* KxAnimatedSplashWindow::GetAnimation() const
{
	return m_Animation.get();
}
void KxAnimatedSplashWindow::SetAnimation(wxAnimation* animation)
{
	m_Animation.reset(animation);
	DoResetAnimation();
	DoUpdateSplash();
}
void KxAnimatedSplashWindow::ResetAnimation()
{
	DoResetAnimation();
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
	DoResetAnimation();
	DoPlay();
	return KxSplashWindow::Show(show);
}
bool KxAnimatedSplashWindow::Destroy()
{
	OnDestroy();
	return KxSplashWindow::Destroy();
}
void KxAnimatedSplashWindow::Play()
{
	DoResetAnimation();
	DoPlay();
}
