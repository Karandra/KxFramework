#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxCOM.h"
class KxTaskScheduler;

struct ITaskDefinition;
struct IRegistrationInfo;
struct IPrincipal;
struct ITaskSettings;
struct IIdleSettings;
struct ITriggerCollection;
struct IActionCollection;
struct IAction;
class KxTaskSchedulerTask
{
	friend class KxTaskScheduler;

	private:
		KxCOMPtr<ITaskDefinition> m_Task;
		KxCOMPtr<IRegistrationInfo> m_RegInfo;
		KxCOMPtr<IPrincipal> m_Principal;
		KxCOMPtr<ITaskSettings> m_Settings;
		KxCOMPtr<IIdleSettings> m_IdleSettings;
		
		KxCOMPtr<ITriggerCollection> m_TriggerCollection;
		KxCOMPtr<IActionCollection> m_ActionCollection;

	private:
		KxTaskSchedulerTask(ITaskDefinition* taskDef = NULL);

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
class KxTaskScheduler
{
	private:
		KxCOMInit m_COMInit;
		KxCOMPtr<ITaskService> m_TaskService;
		KxCOMPtr<ITaskFolder> m_TaskFolder;

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

