#include "KxfPCH.h"
#include "DefaultRPCEvent.h"
#include "DefaultRPCServer.h"
#include "DefaultRPCClient.h"
#include "kxf/IO/NullStream.h"

namespace kxf
{
	IRPCServer* DefaultRPCEvent::GetServer() const
	{
		return m_Server;
	}
	IRPCClient* DefaultRPCEvent::GetClient() const
	{
		return m_Client;
	}

	IInputStream& DefaultRPCEvent::RawGetProcedureResult()
	{
		if (m_ResultStream)
		{
			m_ResultStreamRead.emplace(*m_ResultStream);
			return *m_ResultStreamRead;
		}
		return NullInputStream::Get();
	}
	void DefaultRPCEvent::RawSetProcedureResult(IInputStream& stream)
	{
		m_ResultStream.emplace();
		m_ResultStream->Write(stream);
	}

	IInputStream& DefaultRPCEvent::RawGetProcedureParameters()
	{
		return m_ParametersStream ? *m_ParametersStream : NullInputStream::Get();
	}
	void DefaultRPCEvent::RawSetProcedureParameters(IInputStream& stream)
	{
		m_ParametersStream = &stream;
	}
}
