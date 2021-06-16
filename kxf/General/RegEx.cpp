#include "KxfPCH.h"
#include "RegEx.h"

namespace kxf
{
	size_t RegEx::Replace(String& text, const String& replacement, size_t maxMatches) const
	{
		wxString temp;
		Private::MoveWxString(temp, std::move(text.impl_str()));

		auto result = std::clamp(m_RegEx.Replace(&temp, replacement, maxMatches), 0, std::numeric_limits<int>::max());
		Private::MoveWxString(text.impl_str(), std::move(temp));

		return result;
	}
}
