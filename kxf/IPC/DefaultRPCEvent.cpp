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

	IInputStream& DefaultRPCEvent::GetProcedureResult()
	{
		return m_ResultStream ? *m_ResultStream : NullInputStream::Get();
	}
	void DefaultRPCEvent::SetProcedureResult(IInputStream& stream)
	{
		m_ResultStream = &stream;
	}

	IInputStream& DefaultRPCEvent::GetProcedureParameters()
	{
		return m_ParametersStream ? *m_ParametersStream : NullInputStream::Get();
	}
	void DefaultRPCEvent::SetProcedureParameters(IInputStream& stream)
	{
		m_ParametersStream = &stream;
	}
}
