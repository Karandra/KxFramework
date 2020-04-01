/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/System/COM.h"
class KX_API KxTaskScheduler;

struct ITaskDefinition;
struct IRegistrationInfo;
struct IPrincipal;
struct ITaskSettings;
struct IIdleSettings;
struct ITriggerCollection;
struct IActionCollection;
struct IAction;
class KX_API KxTaskSchedulerTask
{
	friend class KxTaskScheduler;

	private:
		KxFramework::COMPtr<ITaskDefinition> m_Task;
		KxFramework::COMPtr<IRegistrationInfo> m_RegInfo;
		KxFramework::COMPtr<IPrincipal> m_Principal;
		KxFramework::COMPtr<ITaskSettings> m_Settings;
		KxFramework::COMPtr<IIdleSettings> m_IdleSettings;
		
		KxFramework::COMPtr<ITriggerCollection> m_TriggerCollection;
		KxFramework::COMPtr<IActionCollection> m_ActionCollection;

	private:
		KxTaskSchedulerTask(ITaskDefinition* taskDef = nullptr);

	public:
		virtual ~KxTaskSchedulerTask();

	public:
		bool IsOK() const
		{
			return m_Task && m_RegInfo && m_Principal && m_Settings && m_IdleSettings;
		}

		bool SetExecutable(const wxString& path, const wxString& arguments = wxEmptyString, const wxString& workingDirectory = wxEmptyString);
		bool SetTimeTrigger(const wxString& id, const wxDateTime& start, const wxDateTime& end);
		bool SetRegistrationTrigger(const wxString& id, const wxTimeSpan& delay, const wxDateTime& end = wxDefaultDateTime);
		bool DeleteExpiredTaskAfter(const wxTimeSpan& delay);
};

//////////////////////////////////////////////////////////////////////////
struct ITaskService;
struct ITaskFolder;
class KX_API KxTaskScheduler
{
	private:
		KxFramework::COMInitGuard m_COMInit;
		KxFramework::COMPtr<ITaskService> m_TaskService;
		KxFramework::COMPtr<ITaskFolder> m_TaskFolder;

	public:
		KxTaskScheduler(const wxString& folder = wxEmptyString,
						const wxString& serverName = wxEmptyString,
						const wxString& userName = wxEmptyString,
						const wxString& domain = wxEmptyString,
						const wxString& password = wxEmptyString
		);
		virtual ~KxTaskScheduler();

	public:
		bool IsOK() const
		{
			return m_COMInit && m_TaskService && m_TaskFolder;
		}

		KxTaskSchedulerTask NewTask();
		bool SaveTask(const KxTaskSchedulerTask& task, const wxString& name);
		bool DeleteTask(const wxString& taskName);
};
