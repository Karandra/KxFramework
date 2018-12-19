/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxProcess.h"

class KX_API KxProcessPipe;
class KX_API wxStreamTempInputBuffer;
class KX_API KxProcessThread: public wxThread
{
	protected:
		KxProcess* m_EventHandler = nullptr;
		const KxProcessWaitMode m_RunMode = KxPROCESS_RUN_ASYNC;
		const bool m_IsRedirectedIO;
		bool m_HideUI = true;
		bool m_IsProcessCreated = false;
		HANDLE m_Handle = INVALID_HANDLE_VALUE;
		
		wxCriticalSection m_EndCS;
		bool m_EndSignaled = false;

	private:
		bool PrepareRedirection(KxProcessPipe& pipeIn, KxProcessPipe& pipeOut, KxProcessPipe& pipeError, STARTUPINFOW& startupInfo);
		bool EndRedirection(KxProcessPipe& pipeIn, KxProcessPipe& pipeOut, KxProcessPipe& pipeError, wxStreamTempInputBuffer& outBuffer, wxStreamTempInputBuffer& errorBuffer);
		void* ConvertEnvTable(const KxProcessEnvMap& envTable, wxMemoryBuffer& buffer);
		void SendEventSyncAsync(wxEvent* event);
		bool IsProcessAlive() const
		{
			return m_EventHandler != nullptr && !m_EndSignaled;
		}

	protected:
		virtual bool CreateProcess(PROCESS_INFORMATION& processInfo);
		virtual void CloseThreadAndProcess(PROCESS_INFORMATION& processInfo);
		virtual bool WaitProcessEnd(PROCESS_INFORMATION& processInfo);
		virtual bool WaitProcessIdle(PROCESS_INFORMATION& processInfo);
		virtual void SendProcessEndEvent();
		virtual void SendProcessIdleEvent();

		virtual ExitCode Entry() override;

	public:
		KxProcessThread(KxProcess* eventHandler, KxProcessWaitMode runMode, bool hideUI = true, HANDLE processHandle = INVALID_HANDLE_VALUE);
		virtual ~KxProcessThread();

	public:
		wxThreadError RunHere()
		{
			Entry();
			return wxTHREAD_NOT_RUNNING;
		}
		bool IsProcessCreated() const
		{
			return m_IsProcessCreated;
		}
};

class KX_API KxProcessPipe
{
	public:
		enum Direction
		{
			Read,
			Write
		};

	private:
		HANDLE m_PipeHandles[2];

	public:
		KxProcessPipe()
		{
			m_PipeHandles[Read] = INVALID_HANDLE_VALUE;
			m_PipeHandles[Write] = INVALID_HANDLE_VALUE;
		}
		~KxProcessPipe()
		{
			Close();
		}

	public:
		bool Create()
		{
			SECURITY_ATTRIBUTES security;
			security.nLength = sizeof(security);
			security.lpSecurityDescriptor = nullptr;
			security.bInheritHandle = TRUE;

			return ::CreatePipe(&m_PipeHandles[0], &m_PipeHandles[1], &security, 0);
		}
		bool IsOK() const
		{
			return m_PipeHandles[Read] != INVALID_HANDLE_VALUE;
		}
		HANDLE operator[](Direction direction) const
		{
			return m_PipeHandles[direction];
		}
		HANDLE Detach(Direction direction)
		{
			HANDLE pipeHandle = m_PipeHandles[direction];
			m_PipeHandles[direction] = INVALID_HANDLE_VALUE;
			return pipeHandle;
		}
		void Close()
		{
			for (size_t i = 0; i < WXSIZEOF(m_PipeHandles); i++)
			{
				if (m_PipeHandles[i] != INVALID_HANDLE_VALUE)
				{
					::CloseHandle(m_PipeHandles[i]);
					m_PipeHandles[i] = INVALID_HANDLE_VALUE;
				}
			}
		}
};
