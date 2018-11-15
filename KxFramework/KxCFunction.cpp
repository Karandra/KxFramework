#include "KxStdAfx.h"
#include "KxFramework/KxCFunction.h"
#include <type_traits>

#define FFI_BUILDING 1
#include "KxFramework/LibFFI/ffi.h"

#if defined _WIN64
	#pragma comment(lib, "KxFramework/LibFFI/libffi x64.lib")
#else
	#pragma comment(lib, "KxFramework/LibFFI/libffi x86.lib")
#endif

namespace
{
	using Type = KxCFunction::Type;
	using ABI = KxCFunction::ABI;

	constexpr ffi_type* GetFFIType(Type type)
	{
		switch (type)
		{
			case Type::Void:
			{
				return &ffi_type_void;
			}

			case Type::Int8:
			{
				return &ffi_type_sint8;
			}
			case Type::Int16:
			{
				return &ffi_type_sint16;
			}
			case Type::Int32:
			{
				return &ffi_type_sint32;
			}
			case Type::Int64:
			{
				return &ffi_type_sint64;
			}

			case Type::UInt8:
			{
				return &ffi_type_uint8;
			}
			case Type::UInt16:
			{
				return &ffi_type_uint16;
			}
			case Type::UInt32:
			{
				return &ffi_type_uint32;
			}
			case Type::UInt64:
			{
				return &ffi_type_uint64;
			}

			case Type::Pointer:
			{
				return &ffi_type_pointer;
			}

			case Type::Float32:
			{
				return &ffi_type_float;
			}
			case Type::Float64:
			{
				return &ffi_type_double;
			}
		};
		return NULL;
	}
	constexpr Type GetFromFFIType(const ffi_type* type)
	{
		if (type == &ffi_type_void)
		{
			return Type::Void;
		}

		if (type == &ffi_type_sint8)
		{
			return Type::Int8;
		}
		if (type == &ffi_type_sint8)
		{
			return Type::Int8;
		}
		if (type == &ffi_type_sint16)
		{
			return Type::Int16;
		}
		if (type == &ffi_type_sint32)
		{
			return Type::Int32;
		}
		if (type == &ffi_type_sint64)
		{
			return Type::Int64;
		}

		if (type == &ffi_type_uint8)
		{
			return Type::UInt8;
		}
		if (type == &ffi_type_uint8)
		{
			return Type::UInt8;
		}
		if (type == &ffi_type_uint16)
		{
			return Type::UInt16;
		}
		if (type == &ffi_type_uint32)
		{
			return Type::UInt32;
		}
		if (type == &ffi_type_uint64)
		{
			return Type::UInt64;
		}

		if (type == &ffi_type_pointer)
		{
			return Type::Pointer;
		}

		if (type == &ffi_type_float)
		{
			return Type::Float32;
		}
		if (type == &ffi_type_double)
		{
			return Type::Float64;
		}

		return Type::Invalid;
	}
	constexpr ffi_abi GetFFIABI(ABI abi)
	{
		#if _WIN64
		if (abi != ABI::Invalid)
		{
			return FFI_WIN64;
		}
		#else
		switch (abi)
		{
			case ABI::Default:
			case ABI::CDECL_ABI:
			{
				return FFI_MS_CDECL;
			}
			case ABI::STDCALL_ABI:
			{
				return FFI_STDCALL;
			}
			case ABI::THISCALL_ABI:
			{
				return FFI_THISCALL;
			}
		};
		#endif
		return FFI_FIRST_ABI;
	}
}

//////////////////////////////////////////////////////////////////////////
class KxFFI::FunctionData
{
	friend class KxCFunction;

	public:
		constexpr static const size_t ms_MaxParametersCount = 16;

	private:
		ffi_cif m_CInterface;
		ffi_closure* m_Closure = NULL;
		void* m_Code = NULL;
		ABI m_ABI = ABI::Invalid;

		std::array<ffi_type*, ms_MaxParametersCount> m_ArgumentTypes;
		size_t m_ArgumentsCount = 0;
		Type m_ReturnType = Type::Invalid;
		ffi_status m_Status = FFI_OK;

	public:
		FunctionData()
		{
			m_ArgumentTypes.fill(NULL);
		}
};

namespace KxFFI
{
	size_t GetMaxParameterCount()
	{
		return FunctionData::ms_MaxParametersCount;
	}
	size_t GetTypeSize(Type type)
	{
		return GetFFIType(type)->size;
	}
}

//////////////////////////////////////////////////////////////////////////
KxCFunction::KxCFunction()
	:m_FunctionData(std::make_unique<FunctionData>())
{
}
KxCFunction::KxCFunction(KxCFunction&& other)
	:KxCFunction()
{
	*this = std::move(other);
}
KxCFunction::~KxCFunction()
{
	if (m_FunctionData && m_FunctionData->m_Closure)
	{
		ffi_closure_free(m_FunctionData->m_Closure);
	}
}

bool KxCFunction::Create()
{
	m_FunctionData->m_Closure = static_cast<ffi_closure*>(ffi_closure_alloc(sizeof(ffi_closure), &m_FunctionData->m_Code));
	if (m_FunctionData->m_Closure)
	{
		ffi_type** argumentTypes = m_FunctionData->m_ArgumentTypes.data();
		ffi_type* returnType = GetFFIType(m_FunctionData->m_ReturnType);
		ffi_abi abi = GetFFIABI(m_FunctionData->m_ABI);

		m_FunctionData->m_Status = ffi_prep_cif(&m_FunctionData->m_CInterface, abi, m_FunctionData->m_ArgumentsCount, returnType, argumentTypes);
		if (m_FunctionData->m_Status == FFI_OK)
		{
			auto OnCall = [](ffi_cif* cif, void* ret, void** args, void* context)
			{
				reinterpret_cast<KxCFunction*>(context)->Execute(args, ret);
			};

			m_FunctionData->m_Status = ffi_prep_closure_loc(m_FunctionData->m_Closure, &m_FunctionData->m_CInterface, OnCall, this, m_FunctionData->m_Code);
			return m_FunctionData->m_Status == FFI_OK;
		}
	}
	return false;
}
bool KxCFunction::IsOK() const
{
	return m_FunctionData && m_FunctionData->m_Closure && m_FunctionData->m_Code && m_FunctionData->m_ReturnType != Type::Invalid;
}

const void* KxCFunction::GetCode() const
{
	return m_FunctionData->m_Code;
}
size_t KxCFunction::GetCodeSize() const
{
	return m_FunctionData->m_CInterface.bytes;
}

size_t KxCFunction::GetParametersCount() const
{
	return m_FunctionData->m_ArgumentsCount;
}
bool KxCFunction::HasParameters() const
{
	return m_FunctionData->m_ArgumentsCount != 0;
}

bool KxCFunction::AddParameter(Type type)
{
	if (type != Type::Void && m_FunctionData->m_ArgumentsCount + 1 < m_FunctionData->m_ArgumentTypes.size())
	{
		ffi_type* ffiType = GetFFIType(type);
		if (ffiType)
		{
			m_FunctionData->m_ArgumentTypes[m_FunctionData->m_ArgumentsCount] = ffiType;
			m_FunctionData->m_ArgumentsCount++;
			return true;
		}
	}
	return false;
}
Type KxCFunction::GetParameterType(size_t index) const
{
	return GetFromFFIType(m_FunctionData->m_ArgumentTypes[index]);
}

Type KxCFunction::GetReturnType() const
{
	return m_FunctionData->m_ReturnType;
}
void KxCFunction::SetReturnType(Type type)
{
	m_FunctionData->m_ReturnType = type;
}

ABI KxCFunction::GetABI() const
{
	return m_FunctionData->m_ABI;
}
void KxCFunction::SetABI(ABI abi)
{
	m_FunctionData->m_ABI = abi;
}

KxCFunction& KxCFunction::operator=(KxCFunction&& other)
{
	m_FunctionData = std::move(other.m_FunctionData);
	return *this;
}
