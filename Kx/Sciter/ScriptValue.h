#pragma once
#include "Common.h"
#include <utility>

namespace KxFramework::Sciter
{
	enum class ScriptValueType: uint32_t
	{
		None = 0,
		Null,
		Bool,
		Int,
		Float,
		String,
		DateTime,
		Currency,
		Length,
		Array,
		Map,
		Function,
		Bytes,
		Object,
		DOMObject,
		Resource,
		Range,
		Duration,
		Angle,
		Color
	};
	struct ScriptNativeValue
	{
		ScriptValueType Type = ScriptValueType::None;
		uint32_t Value32 = 0;
		uint64_t Value64 = 0;
	};
}

namespace KxFramework::Sciter
{
	class KX_API ScriptValue final
	{
		public:
			using TFunction = std::function<ScriptValue(const ScriptValue* arguments, size_t count)>;
			enum class StringType
			{
				None,
				Symbol,
				Secure,
				Error
			};

		private:
			ScriptNativeValue m_Value;

		private:
			void Init();
			void Clear();
			void Copy(const ScriptNativeValue& other);
			bool IsEqual(const ScriptNativeValue& other) const;
			void AssingString(wxStringView data, StringType type);

		public:
			ScriptValue()
			{
				Init();
			}
			ScriptValue(const ScriptValue& other)
			{
				Init();
				Copy(other.m_Value);
			}
			ScriptValue(const ScriptNativeValue& other)
			{
				Init();
				Copy(other);
			}

			template<class T>
			ScriptValue(T&& value)
			{
				Init();
				*this = std::forward<T>(value);
			}

			~ScriptValue()
			{
				Clear();
			}

		public:
			bool IsNone() const;
			ScriptValueType GetType() const;
			
			const ScriptNativeValue& GetNativeValue() const
			{
				return m_Value;
			}
			ScriptNativeValue& GetNativeValue()
			{
				return m_Value;
			}

			// Retrieving
			wxString GetString() const;
			std::optional<int> GetInt() const;
			std::optional<int64_t> GetInt64() const;
			std::optional<double> GetFloat() const;
			wxDateTime GetDateTime() const;
			wxTimeSpan GetDuration() const;
			Color GetColor() const;
			std::optional<double> GetAngle() const;
			const void* GetBytes(size_t& size) const;
			std::optional<int64_t> GetCurrency() const;

			// Assignment
			ScriptValue& operator=(int value);
			ScriptValue& operator=(int64_t value);
			ScriptValue& operator=(double value);
			ScriptValue& operator=(bool value);
			ScriptValue& operator=(const wxString& value)
			{
				SetString(value);
				return *this;
			}
			ScriptValue& operator=(std::string_view value)
			{
				SetString(wxString::FromUTF8(value.data(), value.length()));
				return *this;
			}
			ScriptValue& operator=(std::wstring_view value)
			{
				AssingString(value, StringType::None);
				return *this;
			}
			ScriptValue& operator=(const char* value)
			{
				*this = std::string_view(value ? value : "");
				return *this;
			}
			ScriptValue& operator=(const wchar_t* value)
			{
				*this = std::wstring_view(value ? value : L"");
				return *this;
			}
			ScriptValue& operator=(const wxDateTime& value);
			ScriptValue& operator=(const wxTimeSpan& value);
			ScriptValue& operator=(const Color& value);
			ScriptValue& SetAngle(double radians);
			ScriptValue& SetBytes(const void* data, size_t size);
			ScriptValue& SetString(const wxString& value, StringType type = StringType::None)
			{
				AssingString(wxStringView(value.wc_str(), value.length()), type);
				return *this;
			}
			ScriptValue& SetCurrency(int64_t value);

			ScriptValue& operator=(const ScriptValue& other)
			{
				Copy(other.m_Value);
				return *this;
			}
			ScriptValue& operator=(const ScriptNativeValue& other)
			{
				Copy(other);
				return *this;
			}

		public:
			explicit operator bool() const
			{
				return !IsNone();
			}
			bool operator!() const
			{
				return IsNone();
			}

			bool operator==(const ScriptValue& other) const
			{
				return IsEqual(other.m_Value);
			}
			bool operator!=(const ScriptValue& other) const
			{
				return !(*this == other);
			}
	};
}
