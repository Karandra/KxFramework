#pragma once
#include "Common.h"

namespace kxf
{
	class WebRequestHeader final
	{
		friend struct std::hash<WebRequestHeader>;

		public:
			static WebRequestHeader UserAgent(String value);
			static WebRequestHeader Referer(String value);
			static WebRequestHeader Cookie(String value);

		private:
			String m_Name;
			String m_Value;

		public:
			WebRequestHeader() = default;
			WebRequestHeader(String name, String value = {})
				:m_Name(std::move(name)), m_Value(std::move(value))
			{
			}
			~WebRequestHeader() = default;

		public:
			bool IsNull() const noexcept
			{
				return m_Name.IsEmpty();
			}
			bool IsEmpty() const noexcept
			{
				return m_Value.IsEmpty();
			}
			bool IsSameAs(const WebRequestHeader& other) const noexcept
			{
				return m_Name == other.m_Name;
			}

			const String& GetName() const& noexcept
			{
				return m_Name;
			}
			String GetName() && noexcept
			{
				return std::move(m_Name);
			}
			void SetName(String name)
			{
				m_Name = std::move(name);
			}

			const String& GetValue() const& noexcept
			{
				return m_Value;
			}
			String GetValue() && noexcept
			{
				return std::move(m_Value);
			}
			void SetValue(String value)
			{
				m_Value = std::move(value);
			}
			bool AddValue(const String& value, WebRequestHeaderFlag separator);

		public:
			String Format() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			bool operator==(const WebRequestHeader& other) const noexcept;
			bool operator!=(const WebRequestHeader& other) const noexcept;
	};
}

namespace std
{
	template<>
	struct hash<kxf::WebRequestHeader> final
	{
		size_t operator()(const kxf::WebRequestHeader& header) const noexcept;
	};
}
