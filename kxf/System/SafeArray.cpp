#include "stdafx.h"
#include "SafeArray.h"
#include "COM.h"

namespace kxf
{
	HResult SafeArray::DoClear() noexcept
	{
		::SafeArrayDestroy(&m_SafeArray);
	}
	HResult SafeArray::DoCopy(const tagSAFEARRAY& other) noexcept
	{
		if (&other != &m_SafeArray)
		{
			HResult hr = HResult::Fail();

			SafeArrayPtr ptr;
			if (hr = ::SafeArrayCopy(const_cast<SAFEARRAY*>(&other), &ptr))
			{
				DoMove(std::move(*ptr.Detach()));
			}
			return hr;
		}
		return HResult::Success();
	}
	HResult SafeArray::DoMove(tagSAFEARRAY&& other) noexcept
	{
		if (&other != &m_SafeArray)
		{
			std::memcpy(&m_SafeArray, &other, sizeof(other));
			std::memset(&other, 0, sizeof(other));
		}
		return HResult::Success();
	}

	HResult SafeArray::AccessData(void**& data) noexcept
	{
		return ::SafeArrayAccessData(&m_SafeArray, data);
	}
	void SafeArray::UnaccessData() noexcept
	{
		::SafeArrayUnaccessData(&m_SafeArray);
	}

	SafeArray::SafeArray() noexcept = default;
	SafeArray::SafeArray(const tagSAFEARRAY& other) noexcept
	{
		DoCopy(other);
	}
	SafeArray::SafeArray(const SafeArray& other) noexcept
	{
		DoCopy(*other.m_SafeArray);
	}
	SafeArray::SafeArray(SafeArray&& other) noexcept
	{
		DoMove(std::move(*other.m_SafeArray));
	}
	SafeArray::SafeArray(SafeArrayPtr&& other) noexcept
	{
		if (DoMove(std::move(*other)))
		{
			other.Detach();
		}
	}
	SafeArray::~SafeArray() noexcept
	{
		DoClear();
	}

	bool SafeArray::IsNull() const noexcept
	{
		VARTYPE type = VT_NULL;
		if (HResult result = ::SafeArrayGetVartype(const_cast<SAFEARRAY*>(&m_SafeArray), &type))
		{
			return type == VT_NULL;
		}
		return false;
	}

	bool SafeArray::IsEmpty() const noexcept
	{
		VARTYPE type = VT_NULL;
		if (HResult result = ::SafeArrayGetVartype(const_cast<SAFEARRAY*>(&m_SafeArray), &type))
		{
			return type == VT_NULL || type == VT_EMPTY;
		}
		return true;
	}
	size_t SafeArray::GetSize(size_t dimension) const noexcept
	{
		decltype(auto) items = const_cast<SAFEARRAY*>(&m_SafeArray);
		LONG lower = 0;
		LONG upper = 0;

		if (HResult(::SafeArrayGetLBound(items, static_cast<UINT>(dimension), &lower)) && (HResult(::SafeArrayGetUBound(items, static_cast<UINT>(dimension), &upper))))
		{
			return upper - lower + 1;
		}
		return 0;
	}
	size_t SafeArray::GetDimensions() const noexcept
	{
		return ::SafeArrayGetDim(const_cast<SAFEARRAY*>(&m_SafeArray));
	}
	SafeArrayType SafeArray::GetType() const noexcept
	{
		VARTYPE type = VT_NULL;
		if (HResult result = ::SafeArrayGetVartype(const_cast<SAFEARRAY*>(&m_SafeArray), &type))
		{
			switch (type)
			{
				case VARENUM::VT_EMPTY:
				{
					return SafeArrayType::None;
				}

				case VARENUM::VT_BOOL:
				{
					return SafeArrayType::Bool;
				}
				case VARENUM::VT_BSTR:
				case VARENUM::VT_LPSTR:
				case VARENUM::VT_LPWSTR:
				{
					return SafeArrayType::String;
				}
				case VARENUM::VT_FILETIME:
				case VARENUM::VT_DATE:
				{
					return SafeArrayType::DateTime;
				}
				case VARENUM::VT_ARRAY|VT_UI1:
				{
					return SafeArrayType::UUID;
				}

				case VARENUM::VT_I1:
				{
					return SafeArrayType::Int8;
				}
				case VARENUM::VT_UI1:
				{
					return SafeArrayType::UInt8;
				}
				case VARENUM::VT_I2:
				{
					return SafeArrayType::Int16;
				}
				case VARENUM::VT_UI2:
				{
					return SafeArrayType::UInt16;
				}
				case VARENUM::VT_I4:
				{
					return SafeArrayType::Int32;
				}
				case VARENUM::VT_UI4:
				{
					return SafeArrayType::UInt32;
				}
				case VARENUM::VT_I8:
				{
					return SafeArrayType::Int64;
				}
				case VARENUM::VT_UI8:
				{
					return SafeArrayType::UInt64;
				}

				case VARENUM::VT_R4:
				{
					return SafeArrayType::Float32;
				}
				case VARENUM::VT_R8:
				{
					return SafeArrayType::Float64;
				}
			};
		}
		return SafeArrayType::Unknown;
	}

	SafeArray& SafeArray::operator=(const SafeArray& other) noexcept
	{
		DoClear();
		DoCopy(*other.m_SafeArray);
		return *this;
	}
	SafeArray& SafeArray::operator=(SafeArray&& other) noexcept
	{
		DoClear();
		DoMove(std::move(*other.m_SafeArray));
		return *this;
	}
	SafeArray& SafeArray::operator=(SafeArrayPtr&& other) noexcept
	{
		DoClear();
		if (DoMove(std::move(*other)))
		{
			other.Detach();
		}
		return *this;
	}
}
