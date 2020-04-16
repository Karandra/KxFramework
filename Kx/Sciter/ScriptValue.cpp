#include "KxStdAfx.h"
#include "ScriptValue.h"
#include "Internal.h"
#include "SciterAPI.h"

namespace KxFramework::Sciter
{
	ScriptValueType MapValueType(uint32_t nativeType)
	{
		switch (nativeType)
		{
			case VALUE_TYPE::T_UNDEFINED:
			{
				return ScriptValueType::None;
			}
			case VALUE_TYPE::T_NULL:
			{
				return ScriptValueType::Null;
			}
			case VALUE_TYPE::T_BOOL:
			{
				return ScriptValueType::Bool;
			}
			case VALUE_TYPE::T_INT:
			{
				return ScriptValueType::Int;
			}
			case VALUE_TYPE::T_FLOAT:
			{
				return ScriptValueType::Float;
			}
			case VALUE_TYPE::T_STRING:
			{
				return ScriptValueType::String;
			}
			case VALUE_TYPE::T_DATE:
			{
				return ScriptValueType::DateTime;
			}
			case VALUE_TYPE::T_CURRENCY:
			{
				return ScriptValueType::Currency;
			}
			case VALUE_TYPE::T_LENGTH:
			{
				return ScriptValueType::Length;
			}
			case VALUE_TYPE::T_ARRAY:
			{
				return ScriptValueType::Array;
			}
			case VALUE_TYPE::T_MAP:
			{
				return ScriptValueType::Map;
			}
			case VALUE_TYPE::T_FUNCTION:
			{
				return ScriptValueType::Function;
			}
			case VALUE_TYPE::T_BYTES:
			{
				return ScriptValueType::Bytes;
			}
			case VALUE_TYPE::T_OBJECT:
			{
				return ScriptValueType::Object;
			}
			case VALUE_TYPE::T_DOM_OBJECT:
			{
				return ScriptValueType::DOMObject;
			}
			case VALUE_TYPE::T_DURATION:
			{
				return ScriptValueType::Duration;
			}
			case VALUE_TYPE::T_ANGLE:
			{
				return ScriptValueType::Angle;
			}
			case VALUE_TYPE::T_COLOR:
			{
				return ScriptValueType::Color;
			}
		};
		return ScriptValueType::None;
	};
	uint32_t MapStringType(ScriptValue::StringType type)
	{
		using StringType = ScriptValue::StringType;

		switch (type)
		{
			case StringType::Secure:
			{
				return VALUE_UNIT_TYPE_STRING::UT_STRING_SECURE;
			}
			case StringType::Error:
			{
				return VALUE_UNIT_TYPE_STRING::UT_STRING_ERROR;
			}
			case StringType::Symbol:
			{
				return VALUE_UNIT_TYPE_STRING::UT_STRING_SYMBOL;
			}
		};
		return VALUE_UNIT_TYPE_STRING::UT_STRING_STRING;
	}
}

namespace KxFramework::Sciter
{
	void ScriptValue::Init()
	{
		GetSciterAPI()->ValueInit(ToSciterScriptValue(m_Value));
	}
	void ScriptValue::Clear()
	{
		GetSciterAPI()->ValueClear(ToSciterScriptValue(m_Value));
	}
	void ScriptValue::Copy(const ScriptNativeValue& other)
	{
		GetSciterAPI()->ValueCopy(ToSciterScriptValue(m_Value), ToSciterScriptValue(const_cast<ScriptNativeValue&>(other)));
	}
	bool ScriptValue::IsEqual(const ScriptNativeValue& other) const
	{
		return &m_Value == &other || GetSciterAPI()->ValueCompare(ToSciterScriptValue(m_Value), ToSciterScriptValue(other)) == HV_OK_TRUE;
	}
	void ScriptValue::AssingString(StringView data, StringType type)
	{
		Clear();
		GetSciterAPI()->ValueStringDataSet(ToSciterScriptValue(m_Value), data.data(), data.length(), MapStringType(type));
	}

	bool ScriptValue::IsNone() const
	{
		return static_cast<uint32_t>(m_Value.Type) == VALUE_TYPE::T_UNDEFINED;
	}
	ScriptValueType ScriptValue::GetType() const
	{
		return MapValueType(static_cast<uint32_t>(m_Value.Type));
	}

	// Retrieving
	String ScriptValue::GetString() const
	{
		const wchar_t* data = nullptr;
		uint32_t length = 0;
		if (GetSciterAPI()->ValueStringData(ToSciterScriptValue(m_Value), &data, &length) == HV_OK)
		{
			return String(data, length);
		}
		return {};
	}
	std::optional<int> ScriptValue::GetInt() const
	{
		int value = -1;
		if (GetSciterAPI()->ValueIntData(ToSciterScriptValue(m_Value), &value) == HV_OK)
		{
			return value;
		}
		return std::nullopt;
	}
	std::optional<int64_t> ScriptValue::GetInt64() const
	{
		int64_t value = -1;
		if (GetSciterAPI()->ValueInt64Data(ToSciterScriptValue(m_Value), &value) == HV_OK)
		{
			return value;
		}
		return std::nullopt;
	}
	std::optional<double> ScriptValue::GetFloat() const
	{
		double value = -1;
		if (GetSciterAPI()->ValueFloatData(ToSciterScriptValue(m_Value), &value) == HV_OK)
		{
			return value;
		}
		return std::nullopt;
	}
	wxDateTime ScriptValue::GetDateTime() const
	{
		if (auto value64 = GetInt64(); value64 && GetType() == ScriptValueType::DateTime)
		{
			FILETIME fileTime = *reinterpret_cast<FILETIME*>(&*value64);
			SYSTEMTIME systemTime = {};
			if (::FileTimeToSystemTime(&fileTime, &systemTime))
			{
				return wxDateTime().SetFromMSWSysTime(systemTime);
			}
		}
		return wxInvalidDateTime;
	}
	wxTimeSpan ScriptValue::GetDuration() const
	{
		if (GetType() == ScriptValueType::Duration)
		{
			return wxTimeSpan::Milliseconds(GetFloat().value_or(0) * 1000);
		}
		return {};
	}
	Color ScriptValue::GetColor() const
	{
		if (auto value = GetInt(); value && GetType() == ScriptValueType::Color)
		{
			return Color().SetCOLORREF(*value);
		}
		return {};
	}
	Angle ScriptValue::GetAngle() const
	{
		if (auto value = GetFloat(); value && GetType() == ScriptValueType::Angle)
		{
			return Angle::FromRadians(static_cast<float>(*value));
		}
		return {};
	}
	const void* ScriptValue::GetBytes(size_t& size) const
	{
		if (GetType() == ScriptValueType::Bytes)
		{
			const void* data = nullptr;
			uint32_t size32 = 0;

			if (GetSciterAPI()->ValueBinaryData(ToSciterScriptValue(m_Value), reinterpret_cast<const BYTE**>(&data), &size32) == HV_OK)
			{
				size = size32;
				return data;
			}
		}

		size = 0;
		return nullptr;
	}
	std::optional<int64_t> ScriptValue::GetCurrency() const
	{
		if (auto value = GetInt64(); value && GetType() == ScriptValueType::Currency)
		{
			return value;
		}
		return std::nullopt;
	}

	// Assignment
	ScriptValue& ScriptValue::operator=(int value)
	{
		Clear();
		GetSciterAPI()->ValueIntDataSet(ToSciterScriptValue(m_Value), value, T_INT, 0);

		return *this;
	}
	ScriptValue& ScriptValue::operator=(int64_t value)
	{
		Clear();
		GetSciterAPI()->ValueInt64DataSet(ToSciterScriptValue(m_Value), value, T_INT, 0);

		return *this;
	}
	ScriptValue& ScriptValue::operator=(double value)
	{
		Clear();
		GetSciterAPI()->ValueIntDataSet(ToSciterScriptValue(m_Value), value, T_FLOAT, 0);

		return *this;
	}
	ScriptValue& ScriptValue::operator=(bool value)
	{
		Clear();
		GetSciterAPI()->ValueIntDataSet(ToSciterScriptValue(m_Value), value ? 1 : 0, T_BOOL, 0);

		return *this;
	}
	ScriptValue& ScriptValue::operator=(const wxDateTime& value)
	{
		Clear();

		SYSTEMTIME systemTime = {};
		FILETIME fileTime = {};
		if (value.GetAsMSWSysDate(&systemTime); ::SystemTimeToFileTime(&systemTime, &fileTime))
		{
			constexpr bool isUTC = false;
			GetSciterAPI()->ValueInt64DataSet(ToSciterScriptValue(m_Value), *reinterpret_cast<uint64_t*>(&fileTime), T_DATE, isUTC);
		}
		return *this;
	}
	ScriptValue& ScriptValue::operator=(const wxTimeSpan& value)
	{
		Clear();

		double seconds = value.GetMilliseconds().GetValue() * 1000;
		GetSciterAPI()->ValueFloatDataSet(ToSciterScriptValue(m_Value), seconds, T_DURATION, 0);

		return *this;
	}
	ScriptValue& ScriptValue::operator=(const Color& value)
	{
		Clear();
		GetSciterAPI()->ValueIntDataSet(ToSciterScriptValue(m_Value), value.GetCOLORREF(), T_COLOR, 0);

		return *this;
	}
	ScriptValue& ScriptValue::SetAngle(Angle angle)
	{
		Clear();
		GetSciterAPI()->ValueFloatDataSet(ToSciterScriptValue(m_Value), angle.ToRadians(), T_ANGLE, 0);

		return *this;
	}
	ScriptValue& ScriptValue::SetBytes(const void* data, size_t size)
	{
		Clear();
		GetSciterAPI()->ValueBinaryDataSet(ToSciterScriptValue(m_Value), reinterpret_cast<const BYTE*>(data), size, T_BYTES, 0);

		return *this;
	}
	ScriptValue& ScriptValue::SetCurrency(int64_t value)
	{
		Clear();
		GetSciterAPI()->ValueInt64DataSet(ToSciterScriptValue(m_Value), value, T_CURRENCY, 0);

		return *this;
	}
}
