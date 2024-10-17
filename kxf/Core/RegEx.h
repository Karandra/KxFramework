#pragma once
#include "Common.h"
#include "String.h"
#include "AlignedStorage.h"
class wxRegEx;

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
			static String EscapeMeta(const String& value);

		private:
			AlignedStorage<wxRegEx, sizeof(void*), alignof(void*)> m_RegEx;

		private:
			void Move(RegEx& other) noexcept;

		public:
			RegEx() noexcept = default;
			RegEx(const String& expression, FlagSet<RegExFlag> flags = {})
			{
				Compile(expression, flags);
			}
			RegEx(RegEx&& other) noexcept
			{
				Move(other);
			}
			RegEx(const RegEx&) = delete;
			~RegEx() noexcept;

		public:
			bool IsNull() const noexcept;
			bool Compile(const String& expression, FlagSet<RegExFlag> flags = {});

			bool Matches(const String& text, FlagSet<RegExCompileFlag> flags = {}) const;
			size_t GetMatchCount() const noexcept;
			String GetMatch(const String& text, size_t index) const;
			size_t EnumMatches(const String& text, std::move_only_function<CallbackCommand(String)> func) const;
			bool GetMatch(size_t& start, size_t& length, size_t index) const noexcept;

			size_t Replace(String& text, const String& replacement, size_t maxMatches = 0) const;
			size_t ReplaceAll(String& text, const String& replacement) const;
			size_t ReplaceFirst(String& text, const String& replacement) const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			RegEx& operator=(RegEx&& other) noexcept
			{
				Move(other);
				return *this;
			}
			RegEx& operator=(const RegEx&) = delete;
	};
};
