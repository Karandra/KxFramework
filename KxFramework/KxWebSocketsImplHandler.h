/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include <string>
#include "KxMainDefs.h"

class KX_API KxWebSocketsClientImplMessageHandler
{
	friend class KxWebSocketClientImpl;

	protected:
		virtual bool OnOpen()
		{
			return true;
		}
		virtual bool OnReady()
		{
			return true;
		}
		virtual bool OnAccept()
		{
			return true;
		}
		virtual void OnClose() {}
		virtual void OnFail() {}

		virtual void OnTextMessage(const char* data, size_t dataLength) {}
		virtual void OnBinaryMessage(const void* data, size_t dataLength) {}

		virtual void OnLogDebug(const char* message) {}
		virtual void OnLogError(const char* message) {}
		virtual void OnLogFatal(const char* message) {}

	public:
		void OnLogDebug(const std::string& message)
		{
			OnLogDebug(message.c_str());
		}
		void OnLogError(const std::string& message)
		{
			OnLogError(message.c_str());
		}
		void OnLogFatal(const std::string& message)
		{
			OnLogFatal(message.c_str());
		}
};
