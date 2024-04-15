#include "KxfPCH.h"
#include "NativeEncodingConverter.h"
#include "kxf/Core/String.h"
#include <Windows.h>

namespace kxf
{
	size_t NativeEncodingConverter::ToMultiByteBuffer(std::span<const wchar_t> source, std::span<std::byte> destination)
	{
		if (m_CodePage >= 0)
		{
			int length = ::WideCharToMultiByte(static_cast<uint32_t>(m_CodePage),
											   m_Flags.ToInt(),
											   source.data(), source.size(),
											   reinterpret_cast<char*>(destination.data()), destination.size(),
											   nullptr,
											   nullptr);

			return length;
		}
		return 0;
	}
	size_t NativeEncodingConverter::ToWideCharBuffer(std::span<const std::byte> source, std::span<wchar_t> destination)
	{
		if (m_CodePage >= 0)
		{
			int length = ::MultiByteToWideChar(static_cast<uint32_t>(m_CodePage),
											   m_Flags.ToInt(),
											   reinterpret_cast<const char*>(source.data()), source.size(),
											   destination.data(), destination.size());

			return length;
		}
		return 0;
	}

	String NativeEncodingConverter::GetEncodingName() const
	{
		CPINFOEXW codePageInfo = {};
		if (m_CodePage >= 0 && ::GetCPInfoExW(static_cast<uint32_t>(m_CodePage), 0, &codePageInfo))
		{
			return codePageInfo.CodePageName;
		}
		return {};
	}
}
