#include "stdafx.h"
#include "IEventLoop.h"
#include "kxf/General/ICoreApplication.h"

namespace
{
	size_t g_EventLoopCount = 0;
	kxf::IEventLoop* g_ActiveEventLoop = nullptr;
}

namespace kxf
{
	size_t IEventLoop::GetEventLoopCount() noexcept
	{
		return g_EventLoopCount;
	}
	IEventLoop* IEventLoop::GetActive() noexcept
	{
		return g_ActiveEventLoop;
	}
	void IEventLoop::SetActive(IEventLoop& eventLoop) noexcept
	{
		g_ActiveEventLoop = &eventLoop;
		eventLoop.OnEnter();
	}

	void IEventLoop::OnEnter()
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			app->OnEventLoopEnter(*this);
		}
	}
	void IEventLoop::OnExit()
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			app->OnEventLoopExit(*this);
		}
	}

	IEventLoop::IEventLoop()
	{
		++g_EventLoopCount;
	}
	IEventLoop::~IEventLoop()
	{
		if (--g_EventLoopCount == 0)
		{
			g_ActiveEventLoop = nullptr;
		}
	}

	bool IEventLoop::IsMain() const
	{
		if (auto app = ICoreApplication::GetInstance())
		{
			return app->GetMainLoop() == this;
		}
		return false;
	}
	bool IEventLoop::IsRunning() const
	{
		return g_ActiveEventLoop == this;
	}
}
