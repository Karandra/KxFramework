#include "KxfPCH.h"
#include "NullStream.h"

namespace
{
	kxf::NullStream g_NullStream;
	kxf::NullInputStream g_NullInputStream;
	kxf::NullOutputStream g_NullOutputStream;
}

namespace kxf
{
	IStream& NullStream::Get()
	{
		return g_NullStream;
	}
	IInputStream& NullInputStream::Get()
	{
		return g_NullInputStream;
	}
	IOutputStream& NullOutputStream::Get()
	{
		return g_NullOutputStream;
	}
}
