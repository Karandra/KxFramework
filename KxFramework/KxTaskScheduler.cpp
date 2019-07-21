/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxTaskScheduler.h"
#include <taskschd.h>
#include <comdef.h>
#include <wincred.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")

namespace Util
{
	_variant_t VariantFromString(const wxString& value = wxEmptyString)
	{
		return value.wc_str();
	}
	_variant_t VariantFromString_Null(const wxString& value = wxEmptyString)
	{
		return value.IsEmpty() ? _variant_t() : VariantFromString(value);
	}

	_bstr_t BstrFromString(const wxString& value = wxEmptyString)
	{
		return value.wc_str();
	}
	_bstr_t BstrFromString_Null(const wxString& value = wxEmptyString)
	{
		return value.IsEmpty() ? _bstr_t() : BstrFromString(value);
	}

	wxString FormatTimeAsTaskBoundary(const wxDateTime& date)
	{
		return date.FormatISOCombined();
	}
	wxString FormatTimeAsTaskDelay(const wxTimeSpan& delay)
	{
		return delay.Format("PT%HH%MM%SS");
	}
}
//////////////////////////////////////////////////////////////////////////
KxTaskSchedulerTask::KxTaskSchedulerTask(ITaskDefinition* taskDef)
	:m_Task(taskDef)
{
	// Get the registration info for setting the identification.
	m_Task->get_RegistrationInfo(&m_RegInfo);

	// Create the principal for the task - these credentials
	// are overwritten with the credentials passed to RegisterTaskDefinition
	m_Task->get_Principal(&m_Principal);

	// Create the settings for the task
	m_Task->get_Settings(&m_Settings);
	if (m_Settings)
	{
		// Create the settings for the task
		m_Settings->get_IdleSettings(&m_IdleSettings);
	}

	// Get the trigger collection to insert the time trigger
	m_Task->get_Triggers(&m_TriggerCollection);

	// Get the task action collection pointer
	m_Task->get_Actions(&m_ActionCollection);
}
KxTaskSchedulerTask::~KxTaskSchedulerTask()
{

}

bool KxTaskSchedulerTask::SetExecutable(const wxString& path, const wxString& arguments, const wxString& workingDirectory)
{
	if (m_ActionCollection)
	{
		KxCOMPtr<IAction> action;
		m_ActionCollection->Create(TASK_ACTION_EXEC, &action);
		if (action)
		{
			KxCOMPtr<IExecAction> execAction;
			if (action->QueryInterface(&execAction); execAction)
			{
				HRESULT res1 = execAction->put_Path(Util::BstrFromString(path));
				HRESULT res2 = execAction->put_Arguments(Util::BstrFromString_Null(arguments));
				HRESULT res3 = execAction->put_WorkingDirectory(Util::BstrFromString_Null(workingDirectory));

				return SUCCEEDED(res1) && SUCCEEDED(res2) && SUCCEEDED(res3);
			}
		}
	}
	return false;
}
bool KxTaskSchedulerTask::SetTimeTrigger(const wxString& id, const wxDateTime& start, const wxDateTime& end)
{
	if (m_TriggerCollection)
	{
		// Add the time trigger to the task
		KxCOMPtr<ITrigger> trigger;
		m_TriggerCollection->Create(TASK_TRIGGER_TIME, &trigger);
		if (trigger)
		{
			KxCOMPtr<ITimeTrigger> timeTrigger;
			trigger->QueryInterface(&timeTrigger);
			if (timeTrigger)
			{
				timeTrigger->put_Id(Util::BstrFromString(id));

				// Set the task to start at a certain time. The time 
				// format should be YYYY-MM-DDTHH:MM:SS(+-)(timezone).
				HRESULT resStart = timeTrigger->put_StartBoundary(Util::BstrFromString(Util::FormatTimeAsTaskBoundary(start)));
				
				HRESULT resEnd = S_OK;
				if (end.IsValid())
				{
					resEnd = timeTrigger->put_EndBoundary(Util::BstrFromString(Util::FormatTimeAsTaskBoundary(end)));
				}

				return SUCCEEDED(resStart) && SUCCEEDED(resEnd);
			}
		}
	}
	return false;
}
bool KxTaskSchedulerTask::SetRegistrationTrigger(const wxString& id, const wxTimeSpan& delay, const wxDateTime& end)
{
	if (m_TriggerCollection)
	{
		// Add the registration trigger to the task.
		KxCOMPtr<ITrigger> trigger;
		m_TriggerCollection->Create(TASK_TRIGGER_REGISTRATION, &trigger);
		if (trigger)
		{
			KxCOMPtr<IRegistrationTrigger> regTrigger;
			trigger->QueryInterface(&regTrigger);
			if (regTrigger)
			{
				regTrigger->put_Id(Util::BstrFromString(id));

				HRESULT res = regTrigger->put_Delay(Util::BstrFromString(Util::FormatTimeAsTaskDelay(delay)));
				
				HRESULT resEnd = S_OK;
				if (end.IsValid())
				{
					resEnd = regTrigger->put_EndBoundary(Util::BstrFromString(Util::FormatTimeAsTaskBoundary(end + delay)));
				}

				return SUCCEEDED(res) && SUCCEEDED(resEnd);
			}
		}
	}
	return false;
}
bool KxTaskSchedulerTask::DeleteExpiredTaskAfter(const wxTimeSpan& delay)
{
	return SUCCEEDED(m_Settings->put_DeleteExpiredTaskAfter(Util::BstrFromString(Util::FormatTimeAsTaskDelay(delay))));
}

//////////////////////////////////////////////////////////////////////////
KxTaskScheduler::KxTaskScheduler(const wxString& folder,
								 const wxString& serverName,
								 const wxString& userName,
								 const wxString& domain,
								 const wxString& password
)
	:m_COMInit(COINIT_APARTMENTTHREADED)
{
	if (m_COMInit)
	{
		// https://docs.microsoft.com/en-us/windows/desktop/TaskSchd/starting-an-executable-at-a-spcific-time
		// Doesn't work, no idea why
		//HRESULT res = ::CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, 0, nullptr);
		
		HRESULT res S_OK;
		if (SUCCEEDED(res))
		{
			res = ::CoCreateInstance(CLSID_TaskScheduler, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskService, m_TaskService.GetPVoid());
			if (SUCCEEDED(res))
			{
				res = m_TaskService->Connect(Util::VariantFromString_Null(serverName),
											 Util::VariantFromString_Null(userName),
											 Util::VariantFromString_Null(domain),
											 Util::VariantFromString_Null(password)
				);
				if (SUCCEEDED(res))
				{
					res = m_TaskService->GetFolder(Util::BstrFromString(folder.IsEmpty() ? wxString("\\") : folder), &m_TaskFolder);
					if (SUCCEEDED(res))
					{
						return;
					}
				}
			}
		}
	}
}
KxTaskScheduler::~KxTaskScheduler()
{
}

KxTaskSchedulerTask KxTaskScheduler::NewTask()
{
	ITaskDefinition* taskDef = nullptr;
	HRESULT res = m_TaskService->NewTask(0, &taskDef);
	if (FAILED(res))
	{
		taskDef = nullptr;
	}
	return KxTaskSchedulerTask(taskDef);
}
bool KxTaskScheduler::SaveTask(const KxTaskSchedulerTask& task, const wxString& name)
{
	KxCOMPtr<IRegisteredTask> registeredTask;
	HRESULT res = m_TaskFolder->RegisterTaskDefinition(Util::BstrFromString(name),
													   task.m_Task.Get(),
													   TASK_CREATE_OR_UPDATE,
													   _variant_t(),
													   _variant_t(),
													   TASK_LOGON_INTERACTIVE_TOKEN,
													   _variant_t(L""),
													   &registeredTask
	);
	return SUCCEEDED(res) && registeredTask;
}

bool KxTaskScheduler::DeleteTask(const wxString& taskName)
{
	return SUCCEEDED(m_TaskFolder->DeleteTask(Util::BstrFromString(taskName), 0));
}
