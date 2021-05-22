#include "KxfPCH.h"
#include "DefaultRPCEvent.h"
#include "DefaultRPCServer.h"
#include "DefaultRPCClient.h"
#include "kxf/IO/NullStream.h"

namespace kxf
{
	void DefaultRPCEvent::RawSetParameters(IInputStream& stream)
	{
		m_ParametersStream = &stream;
		m_ParametersStreamOffset = stream.TellI();
	}
	MemoryInputStream DefaultRPCEvent::RawGetResult()
	{
		if (m_ResultStream)
		{
			return std::move(m_ResultStream);
		}
		return {};
	}

	IRPCServer* DefaultRPCEvent::GetServer() const
	{
		return m_Server;
	}
	IRPCClient* DefaultRPCEvent::GetClient() const
	{
		return m_Client;
	}

	IInputStream& DefaultRPCEvent::RawGetParameters()
	{
		if (m_ParametersStream)
		{
			m_ParametersStream->SeekI(m_ParametersStreamOffset, IOStreamSeek::FromStart);
			return *m_ParametersStream;
		}
		return NullInputStream::Get();
	}
	void DefaultRPCEvent::RawSetResult(IInputStream& stream)
	{
		m_ResultStream = {};
		m_ResultStream.Write(stream);
	}
}
