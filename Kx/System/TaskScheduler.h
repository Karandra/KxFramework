#pragma once
#include "Common.h"
#include "COM.h"
#include "ErrorCodeValue.h"
#include "Kx/General/TimeSpan.h"
#include "Kx/FileSystem/FSPath.h"
#include "Kx/Crypto/SecretValue.h"
struct ITaskDefinition;
struct IRegistrationInfo;
struct IPrincipal;
struct ITaskSettings;
struct IIdleSettings;
struct ITriggerCollection;
struct IActionCollection;
struct IAction;
struct ITaskService;
struct ITaskFolder;

namespace KxFramework::System
{
	class KX_API ScheduledTask final
	{
		friend class TaskScheduler;

		private:
			COMPtr<ITaskDefinition> m_Task;
			COMPtr<IRegistrationInfo> m_RegInfo;
			COMPtr<IPrincipal> m_Principal;
			COMPtr<ITaskSettings> m_Settings;
			COMPtr<IIdleSettings> m_IdleSettings;
			
			COMPtr<ITriggerCollection> m_TriggerCollection;
			COMPtr<IActionCollection> m_ActionCollection;

		private:
			ScheduledTask(COMPtr<ITaskDefinition> taskDefinition);

		public:
			ScheduledTask() noexcept;
			~ScheduledTask() noexcept;

		public:
			bool IsNull() const noexcept
			{
				return !m_Task || !m_RegInfo || !m_Principal || !m_Settings || !m_IdleSettings;
			}

			HResult SetExecutable(const FSPath& path, const String& arguments = {}, const FSPath& workingDirectory = {});
			HResult SetTimeTrigger(const String& id, const wxDateTime& start, const wxDateTime& end);
			HResult SetRegistrationTrigger(const String& id, const TimeSpan& delay, const wxDateTime& end = {});
			HResult DeleteExpiredTaskAfter(const TimeSpan& delay);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}

namespace KxFramework::System
{
	class KX_API TaskScheduler final
	{
		private:
			COMPtr<ITaskService> m_TaskService;
			COMPtr<ITaskFolder> m_TaskFolder;

		public:
			TaskScheduler(const FSPath& taskFolder = {},
						  const String& serverName = {},
						  const String& userName = {},
						  const String& domain = {},
						  const SecretValue& password = {}
			);
			~TaskScheduler() noexcept;

		public:
			bool IsNull() const noexcept
			{
				return !m_TaskService || !m_TaskFolder;
			}

			ScheduledTask NewTask();
			HResult SaveTask(const ScheduledTask& task, const String& name);
			HResult DeleteTask(const String& taskName);

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
