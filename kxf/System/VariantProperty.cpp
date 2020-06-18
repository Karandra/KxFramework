#include "stdafx.h"
#include "VariantProperty.h"
#include "COM.h"
#include "kxf/Utility/CallAtScopeExit.h"
#include <propvarutil.h>
#include <propidlbase.h>
#pragma comment(lib, "Propsys.lib")

namespace
{
	kxf::HResult ClearPropVariant(PROPVARIANT& prop) noexcept
	{
		if (prop.vt == VT_EMPTY)
		{
			return S_OK;
		}

		switch (prop.vt)
		{
			case VT_UI1:
			case VT_I1:
			case VT_I2:
			case VT_UI2:
			case VT_BOOL:
			case VT_I4:
			case VT_UI4:
			case VT_R4:
			case VT_INT:
			case VT_UINT:
			case VT_ERROR:
			case VT_FILETIME:
			case VT_UI8:
			case VT_R8:
			case VT_CY:
			case VT_DATE:
			{
				prop.vt = VT_EMPTY;
				prop.wReserved1 = 0;
				return S_OK;
			}
		};
		return ::VariantClear(reinterpret_cast<VARIANTARG*>(&prop));
	}
	
	template<class T>
	constexpr int SimpleCompare(T a, T b) noexcept
	{
		if (a == b)
		{
			return 0;
		}
		else
		{
			return a < b ? -1 : 1;
		}
	}
}

namespace kxf
{
	HResult VariantProperty::DoClear() noexcept
	{
		HResult hr = ClearPropVariant(*m_Value);
		if (!hr)
		{
			m_Value->vt = VT_ERROR;
			m_Value->scode = *hr;
		}
		return hr;
	}
	HResult VariantProperty::DoCopy(const tagPROPVARIANT& other)
	{
		::VariantClear(reinterpret_cast<VARIANTARG*>(&m_Value));
		m_Value->vt = VT_EMPTY;

		switch (other.vt)
		{
			case VT_UI1:
			case VT_I1:
			case VT_I2:
			case VT_UI2:
			case VT_BOOL:
			case VT_I4:
			case VT_UI4:
			case VT_R4:
			case VT_INT:
			case VT_UINT:
			case VT_ERROR:
			case VT_FILETIME:
			case VT_UI8:
			case VT_R8:
			case VT_CY:
			case VT_DATE:
			{
				std::memmove(reinterpret_cast<PROPVARIANT*>(&m_Value), &other, sizeof(PROPVARIANT));
				return S_OK;
			}
		};

		HResult hr = ::VariantCopy(reinterpret_cast<tagVARIANT*>(this), reinterpret_cast<const tagVARIANT*>(&other));
		if (!hr)
		{
			if (*hr == E_OUTOFMEMORY)
			{
				throw std::bad_alloc();
			}
			m_Value->vt = VT_ERROR;
			m_Value->scode = *hr;
		}
	}
	void VariantProperty::DoMove(tagPROPVARIANT&& other) noexcept
	{
		if (&other != &m_Value)
		{
			std::memcpy(reinterpret_cast<PROPVARIANT*>(&m_Value), &other, sizeof(PROPVARIANT));

			std::memset(&other, 0, sizeof(PROPVARIANT));
			other.vt = VT_EMPTY;
			other.wReserved1 = 0;
		}
	}

	void VariantProperty::AssignBool(bool value) noexcept
	{
		AssignSimpleValue<VT_BOOL>(m_Value->boolVal, value ? VARIANT_TRUE : VARIANT_FALSE);
	}
	void VariantProperty::AssignUUID(const NativeUUID& value)
	{
		::InitVariantFromBuffer(&value, sizeof(value), reinterpret_cast<VARIANT*>(&m_Value));
	}
	void VariantProperty::AssignString(std::string_view value)
	{
		DoClear();

		m_Value->vt = VT_BSTR;
		m_Value->wReserved1 = 0;
		m_Value->bstrVal = ::SysAllocStringByteLen(nullptr, static_cast<UINT>(value.size() * sizeof(OLECHAR)));
		if (!m_Value->bstrVal)
		{
			throw std::bad_alloc();
		}
		else
		{
			for (size_t i = 0; i <= value.size(); i++)
			{
				m_Value->bstrVal[i] = value[i];
			}
		}
	}
	void VariantProperty::AssignString(std::wstring_view value)
	{
		DoClear();

		m_Value->vt = VT_BSTR;
		m_Value->wReserved1 = 0;
		m_Value->bstrVal = ::SysAllocString(value.data());
		if (!m_Value->bstrVal)
		{
			throw std::bad_alloc();
		}
	}
	void VariantProperty::AssignDateTime(const DateTime& value) noexcept
	{
		if (value)
		{
			AssignSimpleValue<VT_FILETIME>(m_Value->filetime, value.GetFileTime(TimeZone::UTC));
		}
		else
		{
			DoClear();
		}
	}

	void VariantProperty::AssignInt(int8_t value) noexcept
	{
		AssignSimpleValue<VT_I1>(m_Value->cVal, value);
	}
	void VariantProperty::AssignInt(int16_t value) noexcept
	{
		AssignSimpleValue<VT_I2>(m_Value->iVal, value);
	}
	void VariantProperty::AssignInt(int32_t value) noexcept
	{
		AssignSimpleValue<VT_I4>(m_Value->lVal, value);
	}
	void VariantProperty::AssignInt(int64_t value) noexcept
	{
		AssignSimpleValue<VT_I8>(m_Value->hVal.QuadPart, value);
	}
	void VariantProperty::AssignInt(uint8_t value) noexcept
	{
		AssignSimpleValue<VT_UI1>(m_Value->bVal, value);
	}
	void VariantProperty::AssignInt(uint16_t value) noexcept
	{
		AssignSimpleValue<VT_UI2>(m_Value->uiVal, value);
	}
	void VariantProperty::AssignInt(uint32_t value) noexcept
	{
		AssignSimpleValue<VT_UI4>(m_Value->ulVal, value);
	}
	void VariantProperty::AssignInt(uint64_t value) noexcept
	{
		AssignSimpleValue<VT_UI8>(m_Value->uhVal.QuadPart, value);
	}

	void VariantProperty::AssignFloat(float value) noexcept
	{
		AssignSimpleValue<VT_R4>(m_Value->fltVal, value);
	}
	void VariantProperty::AssignFloat(double value) noexcept
	{
		AssignSimpleValue<VT_R8>(m_Value->dblVal, value);
	}

	std::optional<int8_t> VariantProperty::RetrieveInt8() const noexcept
	{
		return RetrieveSimpleValue<VT_I1>(m_Value->cVal);
	}
	std::optional<int16_t> VariantProperty::RetrieveInt16() const noexcept
	{
		return RetrieveSimpleValue<VT_I2>(m_Value->iVal);
	}
	std::optional<int32_t> VariantProperty::RetrieveInt32() const noexcept
	{
		return RetrieveSimpleValue<VT_I4>(m_Value->lVal);
	}
	std::optional<int64_t> VariantProperty::RetrieveInt64() const noexcept
	{
		return RetrieveSimpleValue<VT_I8>(m_Value->hVal.QuadPart);
	}
	std::optional<uint8_t> VariantProperty::RetrieveUInt8() const noexcept
	{
		return RetrieveSimpleValue<VT_UI1>(m_Value->bVal);
	}
	std::optional<uint16_t> VariantProperty::RetrieveUInt16() const noexcept
	{
		return RetrieveSimpleValue<VT_UI2>(m_Value->uiVal);
	}
	std::optional<uint32_t> VariantProperty::RetrieveUInt32() const noexcept
	{
		return RetrieveSimpleValue<VT_UI4>(m_Value->ulVal);
	}
	std::optional<uint64_t> VariantProperty::RetrieveUInt64() const noexcept
	{
		return RetrieveSimpleValue<VT_UI8>(m_Value->uhVal.QuadPart);
	}

	std::optional<float> VariantProperty::RetrieveFloat32() const noexcept
	{
		return RetrieveSimpleValue<VT_R4>(m_Value->fltVal);
	}
	std::optional<double> VariantProperty::RetrieveFloat64() const noexcept
	{
		return RetrieveSimpleValue<VT_R8>(m_Value->fltVal);
	}

	VariantProperty::VariantProperty() noexcept
	{
		m_Value->vt = VT_EMPTY;
		m_Value->wReserved1 = 0;
	}
	VariantProperty::~VariantProperty() noexcept
	{
		DoClear();
	}

	int VariantProperty::GetNativeType() const noexcept
	{
		return m_Value->vt;
	}
	void VariantProperty::SetNativeType(int nativeType) noexcept
	{
		m_Value->vt = nativeType;
	}
	
	bool VariantProperty::IsEmpty() const noexcept
	{
		return m_Value->vt == VARENUM::VT_EMPTY;
	}
	VariantPropertyType VariantProperty::GetType() const noexcept
	{
		switch (m_Value->vt)
		{
			case VARENUM::VT_EMPTY:
			{
				return VariantPropertyType::None;
			}

			case VARENUM::VT_BOOL:
			{
				return VariantPropertyType::Bool;
			}
			case VARENUM::VT_BSTR:
			case VARENUM::VT_LPSTR:
			case VARENUM::VT_LPWSTR:
			{
				return VariantPropertyType::String;
			}
			case VARENUM::VT_FILETIME:
			case VARENUM::VT_DATE:
			{
				return VariantPropertyType::DateTime;
			}
			case VARENUM::VT_ARRAY|VT_UI1:
			{
				return VariantPropertyType::UUID;
			}

			case VARENUM::VT_I1:
			{
				return VariantPropertyType::Int8;
			}
			case VARENUM::VT_UI1:
			{
				return VariantPropertyType::UInt8;
			}
			case VARENUM::VT_I2:
			{
				return VariantPropertyType::Int16;
			}
			case VARENUM::VT_UI2:
			{
				return VariantPropertyType::UInt16;
			}
			case VARENUM::VT_I4:
			{
				return VariantPropertyType::Int32;
			}
			case VARENUM::VT_UI4:
			{
				return VariantPropertyType::UInt32;
			}
			case VARENUM::VT_I8:
			{
				return VariantPropertyType::Int64;
			}
			case VARENUM::VT_UI8:
			{
				return VariantPropertyType::UInt64;
			}

			case VARENUM::VT_R4:
			{
				return VariantPropertyType::Float32;
			}
			case VARENUM::VT_R8:
			{
				return VariantPropertyType::Float64;
			}
		}
		return VariantPropertyType::Unknown;
	}
	int VariantProperty::Compare(const VariantProperty& other) const noexcept
	{
		auto CompareDefault = [&]()
		{
			return ::PropVariantCompareEx(*m_Value, *other.m_Value, PVCU_DEFAULT, PVCF_USESTRCMP);
		};

		if (m_Value->vt != m_Value->vt)
		{
			return CompareDefault();
		}

		switch (m_Value->vt)
		{
			case VT_EMPTY:
			{
				return 0;
			}

			case VT_BOOL:
			{
				return -SimpleCompare(m_Value->boolVal, other.m_Value->boolVal);
			}
			case VT_BSTR:
			{
				if (m_Value->bstrVal && other.m_Value->bstrVal)
				{
					using Traits = std::char_traits<OLECHAR>;
					return Traits::compare(m_Value->bstrVal, other.m_Value->bstrVal, std::min(Traits::length(m_Value->bstrVal), Traits::length(other.m_Value->bstrVal)));
				}
				return SimpleCompare(m_Value->bstrVal, other.m_Value->bstrVal);
			}
			case VT_LPSTR:
			{
				return ::PropVariantCompareEx(*m_Value, *other.m_Value, PVCU_DEFAULT, PVCF_USESTRCMPC);
			}
			case VT_LPWSTR:
			{
				return ::PropVariantCompareEx(*m_Value, *other.m_Value, PVCU_DEFAULT, PVCF_USESTRCMP);
			}
			case VT_FILETIME:
			{
				return ::CompareFileTime(&m_Value->filetime, &other.m_Value->filetime);
			}
			case VT_DATE:
			{
				return ::SimpleCompare(m_Value->date, other.m_Value->date);
			}
			case VT_CLSID:
			{
				return ::SimpleCompare(m_Value->date, other.m_Value->date);
			}

			case VT_I1:
			{
				return SimpleCompare(m_Value->cVal, other.m_Value->cVal);
			}
			case VT_UI1:
			{
				return SimpleCompare(m_Value->bVal, other.m_Value->bVal);
			}
			case VT_I2:
			{
				return SimpleCompare(m_Value->iVal, other.m_Value->iVal);
			}
			case VT_UI2:
			{
				return SimpleCompare(m_Value->uiVal, other.m_Value->uiVal);
			}
			case VT_I4:
			{
				return SimpleCompare(m_Value->lVal, other.m_Value->lVal);
			}
			case VT_UI4:
			{
				return SimpleCompare(m_Value->ulVal, other.m_Value->ulVal);
			}
			case VT_I8:
			{
				return SimpleCompare(m_Value->hVal.QuadPart, other.m_Value->hVal.QuadPart);
			}
			case VT_UI8:
			{
				return SimpleCompare(m_Value->uhVal.QuadPart, other.m_Value->uhVal.QuadPart);
			}

			case VT_R4:
			{
				return SimpleCompare(m_Value->fltVal, other.m_Value->fltVal);
			}
			case VT_R8:
			{
				return SimpleCompare(m_Value->dblVal, other.m_Value->dblVal);
			}
		};
		return CompareDefault();
	}

	std::optional<bool> VariantProperty::ToBool() const noexcept
	{
		if (m_Value->vt == VT_BOOL)
		{
			return m_Value->boolVal != VARIANT_FALSE;
		}
		return {};
	}
	std::optional<NativeUUID> VariantProperty::ToUUID() const noexcept
	{
		if (m_Value->vt == (VT_ARRAY|VT_I1))
		{
			NativeUUID uuid;
			if (HResult(::PropVariantToBuffer(*m_Value, &uuid, sizeof(uuid))))
			{
				return uuid;
			}
		}
		return {};
	}
	std::optional<String> VariantProperty::ToString() const
	{
		switch (m_Value->vt)
		{
			case VT_BSTR:
			{
				return m_Value->bstrVal;
			}
			case VT_LPSTR:
			{
				return m_Value->pszVal;
			}
			case VT_LPWSTR:
			{
				return m_Value->pwszVal;
			}
		};
		return {};
	}
	std::optional<DateTime> VariantProperty::ToDateTime() const noexcept
	{
		FILETIME fileTime = {};
		if (HResult(::PropVariantToFileTime(*m_Value, PSTF_LOCAL, &fileTime)))
		{
			return DateTime().SetFileTime(fileTime);
		}
		return {};
	}
}
