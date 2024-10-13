#include "KxfPCH.h"
#include "CFunctionHook.h"
#include "kxf/Log/ScopedLogger.h"
#include "kxf/Log/Categories.h"
#include <detours/detours.h>
#include <detours/detver.h>

namespace kxf::LogCategory
{
	KX_DefineLogModuleCategory(kxf, Detours);
}

namespace kxf
{
	void CFunctionHook::OnAttached(CFunctionHookTransaction& transaction)
	{
		m_Attached = true;
	}
	void CFunctionHook::OnDetached(CFunctionHookTransaction& transaction)
	{
		m_Attached = false;
	}

	CFunctionHook::~CFunctionHook()
	{
		if (m_Attached)
		{
			Log::WarningCategory(LogCategory::kxf_Detours, "Destroying CFunctionHook without detaching it first: name=[{}], original address=[{}], hook address=[{}]", m_Name, m_OriginalAddress, m_HookAddress);
		}
	}
}

namespace kxf
{
	bool CFunctionHookTransaction::DoAttachHook(CFunctionHook& hook, void* originalPtr, void* hookPtr, String name)
	{
		if (m_State == State::Ready && originalPtr && hookPtr && originalPtr != hookPtr)
		{
			m_LastError = ::DetourAttach(reinterpret_cast<void**>(originalPtr), hookPtr);
			if (m_LastError.IsSuccess())
			{
				hook.m_Name = std::move(name);
				hook.m_OriginalAddress = originalPtr;
				hook.m_HookAddress = hookPtr;
				hook.OnAttached(*this);

				return true;
			}
			else
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourAttach: {}", m_LastError);
			}
		}
		return false;
	}
	bool CFunctionHookTransaction::DoDetachHook(CFunctionHook& hook)
	{
		if (m_State == State::Ready && hook.IsAttached())
		{
			m_LastError = ::DetourDetach(reinterpret_cast<void**>(hook.m_OriginalAddress), hook.m_HookAddress);
			if (m_LastError.IsSuccess())
			{
				hook.OnDetached(*this);
				return true;
			}
			else
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourDetach: {}", m_LastError);
			}
		}
		return false;
	}

	CFunctionHookTransaction::CFunctionHookTransaction(CFunctionHookController& controller, RunningSystemThread thread)
		:m_Controller(&controller), m_Thread(std::move(thread))
	{
		if (m_Thread)
		{
			m_LastError = ::DetourTransactionBegin();
			m_State = State::Wait;

			if (m_LastError.IsSuccess())
			{
				m_LastError = ::DetourUpdateThread(m_Thread.GetHandle());
				if (m_LastError.IsSuccess())
				{
					m_State = State::Ready;
				}
				else
				{
					Log::ErrorCategory(LogCategory::kxf_Detours, "DetourUpdateThread: {}", m_LastError);
				}
			}
			else
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourTransactionBegin: {}", m_LastError);
			}
		}
		else
		{
			m_LastError = ERROR_INVALID_PARAMETER;
			m_State = State::Unknown;
			Log::ErrorCategory(LogCategory::kxf_Detours, "Invalid thread");
		}
	}
	CFunctionHookTransaction::~CFunctionHookTransaction()
	{
		if (m_State == State::Ready)
		{
			Commit();
		}
		else if (m_State == State::Wait)
		{
			Abort();
		}
	}

	bool CFunctionHookTransaction::Commit()
	{
		if (m_State == State::Ready)
		{
			m_LastError = ::DetourTransactionCommit();
			m_State = State::Committed;

			if (m_LastError.IsFail())
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourTransactionCommit: {}", m_LastError);
			}
			return m_LastError.IsSuccess();
		}
		return false;
	}
	bool CFunctionHookTransaction::Abort()
	{
		if (m_State == State::Wait || m_State == State::Ready)
		{
			m_LastError = ::DetourTransactionAbort();
			m_State = State::Aborted;

			if (m_LastError.IsFail())
			{
				Log::ErrorCategory(LogCategory::kxf_Detours, "DetourTransactionCommit: {}", m_LastError);
			}
			return m_LastError.IsSuccess();
		}
		return false;
	}
}

namespace kxf
{
	CFunctionHookController::CFunctionHookController()
	{
		m_LastError = Win32Error::Success();
		if (!::DetourRestoreAfterWith())
		{
			m_LastError = Win32Error::GetLastError();
			Log::ErrorCategory(LogCategory::kxf_Detours, "DetourRestoreAfterWith: {}", m_LastError);
		}

		GetInstance();
	}
	CFunctionHookController::~CFunctionHookController()
	{
		// Nothing to uninitialize
	}

	CFunctionHookTransaction CFunctionHookController::NewTransaction(RunningSystemThread thread)
	{
		return CFunctionHookTransaction(*this, std::move(thread));
	}
}
