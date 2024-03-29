#pragma once
#include "Common.h"
#include "String.h"
#include "kxf/Utility/Common.h"
#include <wx/regex.h>

namespace kxf
{
	enum class RegExFlag: uint32_t
	{
		None = 0,

		IgnoreCase = 1 << 0,
		NoSubstitution = 1 << 1,
		NewLine = 1 << 2,
	};
	KxFlagSet_Declare(RegExFlag);

	enum class RegExCompileFlag: uint32_t
	{
		None = 0,

		NotBegin = 1 << 0,
		NotEnd = 1 << 1
	};
	KxFlagSet_Declare(RegExCompileFlag);
}

namespace kxf
{
	class KX_API RegEx final
	{
		public:
			static String EscapeMeta(const String& value)
			{
				return wxRegEx::QuoteMeta(value);
			}

		private:
			wxRegEx m_RegEx;

		public:
			RegEx() = default;
			RegEx(const String& expression, FlagSet<RegExFlag> flags = {})
			{
				Compile(expression, flags);
			}
			RegEx(const RegEx&) = delete;

		public:
			bool IsValid() const noexcept
			{
				return m_RegEx.IsValid();
			}
			bool Compile(const String& expression, FlagSet<RegExFlag> flags = {})
			{
				constexpr auto MapRegExFlag = [](FlagSet<RegExFlag> flags) noexcept
				{
					FlagSet<int> nativeFlags;
					nativeFlags.Add(wxRE_ICASE, flags & RegExFlag::IgnoreCase);
					nativeFlags.Add(wxRE_NOSUB, flags & RegExFlag::NoSubstitution);
					nativeFlags.Add(wxRE_NEWLINE, flags & RegExFlag::NewLine);

					return nativeFlags;
				};

				return m_RegEx.Compile(expression, wxRE_EXTENDED|wxRE_ADVANCED|*MapRegExFlag(flags));
			}

			bool Matches(const String& text, FlagSet<RegExCompileFlag> flags = {}) const
			{
				constexpr auto MapRegExCompileFlag = [](FlagSet<RegExCompileFlag> flags) noexcept
				{
					FlagSet<int> nativeFlags;
					nativeFlags.Add(wxRE_NOTBOL, flags & RegExCompileFlag::NotBegin);
					nativeFlags.Add(wxRE_NOTEOL, flags & RegExCompileFlag::NotEnd);

					return nativeFlags;
				};

				return m_RegEx.Matches(text, *MapRegExCompileFlag(flags));
			}
			size_t GetMatchCount() const noexcept
			{
				return m_RegEx.GetMatchCount();
			}
			String GetMatch(const String& text, size_t index) const
			{
				return m_RegEx.GetMatch(text, index);
			}
			bool GetMatch(size_t& start, size_t& length, size_t index) const noexcept
			{
				return m_RegEx.GetMatch(&start, &length, index);
			}

			size_t Replace(String& text, const String& replacement, size_t maxMatches = 0) const;
			size_t ReplaceAll(String& text, const String& replacement) const
			{
				return Replace(text, replacement, 0);
			}
			size_t ReplaceFirst(String& text, const String& replacement) const
			{
				return Replace(text, replacement, 1);
			}

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}

			RegEx& operator=(const RegEx&) = delete;
	};
};
