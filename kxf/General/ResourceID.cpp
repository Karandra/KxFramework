#include "KxfPCH.h"
#include "ResourceID.h"


namespace kxf
{
	bool ResourceID::IsNull() const noexcept
	{
		return m_Value.IsNull();
	}

	bool ResourceID::HasScheme() const
	{
		return m_Value.HasScheme();
	}
	kxf::String ResourceID::GetScheme() const
	{
		return m_Value.GetScheme();
	}
	kxf::String ResourceID::GetPath() const
	{
		String result = m_Value.GetServer();
		String path = m_Value.GetPath();

		if (!result.EndsWith('/') && !path.StartsWith('/'))
		{
			result += '/';
		}
		result += path;

		return result;
	}

	kxf::String ResourceID::ToString() const
	{
		return m_Value.BuildURI();
	}
}
