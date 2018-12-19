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
	using TypeID = KxCFunction::TypeID;
	using ABI = KxCFunction::ABI;

	constexpr ffi_type* GetFFIType(TypeID type)
	{
		switch (type)
		{
			case TypeID::Void:
			{
				return &ffi_type_void;
			}

			case TypeID::Int8:
			{
				return &ffi_type_sint8;
			}
			case TypeID::Int16:
			{
				return &ffi_type_sint16;
			}
			case TypeID::Int32:
			{
				return &ffi_type_sint32;
			}
			case TypeID::Int64:
			{
				return &ffi_type_sint64;
			}

			case TypeID::UInt8:
			{
				return &ffi_type_uint8;
			}
			case TypeID::UInt16:
			{
				return &ffi_type_uint16;
			}
			case TypeID::UInt32:
			{
				return &ffi_type_uint32;
			}
			case TypeID::UInt64:
			{
				return &ffi_type_uint64;
			}

			case TypeID::Pointer:
			{
				return &ffi_type_pointer;
			}

			case TypeID::Float32:
			{
				return &ffi_type_float;
			}
			case TypeID::Float64:
			{
				return &ffi_type_double;
			}
		};
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
namespace KxFFI
{
	struct CClosure
	{
		uint8_t m_Trampoline[FFI_TRAMPOLINE_SIZE] = {0};
		CInterface* m_CInterface = nullptr;
		CClosureFunction m_Function = nullptr;
		KxCFunction* m_Context = nullptr;
	};
	void CClosureCall(KxFFI::CInterface* cif, void* returnValue, void** arguments, KxCFunction* context)
	{
		context->Execute(arguments, returnValue);
	};

	size_t GetTypeSize(TypeID type)
	{
		return GetFFIType(type)->size;
	}
}

//////////////////////////////////////////////////////////////////////////
KxCFunction::~KxCFunction()
{
	if (m_Closure)
	{
		ffi_closure_free(m_Closure);
	}
}

bool KxCFunction::IsOK() const
{
	// Layout
	static_assert(sizeof(CInterface) == sizeof(ffi_cif), "[KxCFunction] CInterface invalid layout");
	static_assert(sizeof(CType) == sizeof(ffi_type), "[KxCFunction] CType invalid layout");
	static_assert(sizeof(CStatus) == sizeof(ffi_status), "[KxCFunction] CStatus invalid layout");
	static_assert(sizeof(CClosure) == sizeof(CClosure), "[KxCFunction] CClosure invalid layout");

	// Types
	static_assert((int)TypeID::Void == FFI_TYPE_VOID);
	static_assert((int)TypeID::Float32 == FFI_TYPE_FLOAT);
	static_assert((int)TypeID::Float64 == FFI_TYPE_DOUBLE);
	static_assert((int)TypeID::Int8 == FFI_TYPE_SINT8);
	static_assert((int)TypeID::UInt8 == FFI_TYPE_UINT8);
	static_assert((int)TypeID::Int16 == FFI_TYPE_SINT16);
	static_assert((int)TypeID::UInt16 == FFI_TYPE_UINT16);
	static_assert((int)TypeID::Int32 == FFI_TYPE_SINT32);
	static_assert((int)TypeID::UInt32 == FFI_TYPE_UINT32);
	static_assert((int)TypeID::Int64 == FFI_TYPE_SINT64);
	static_assert((int)TypeID::UInt64 == FFI_TYPE_UINT64);
	static_assert((int)TypeID::Pointer == FFI_TYPE_POINTER);

	// ABI
	static_assert((int)ABI::Default == FFI_DEFAULT_ABI);
	#if _WIN64
	static_assert((int)ABI::X64 == FFI_WIN64);
	#else
	static_assert((int)ABI::SysV == FFI_SYSV);
	static_assert((int)ABI::StdCall == FFI_STDCALL);
	static_assert((int)ABI::ThisCall == FFI_THISCALL);
	static_assert((int)ABI::FastCall == FFI_FASTCALL);
	static_assert((int)ABI::CDecl == FFI_MS_CDECL);
	static_assert((int)ABI::Pascal == FFI_PASCAL);
	static_assert((int)ABI::Register == FFI_REGISTER);
	#endif
	

	return m_Closure && m_Code && m_CInterface.m_ReturnType && m_Status == CStatus::OK;
}
bool KxCFunction::Create()
{
	m_Closure = reinterpret_cast<CClosure*>(ffi_closure_alloc(sizeof(ffi_closure), &m_Code));
	if (m_Closure)
	{
		ffi_cif* cif = reinterpret_cast<ffi_cif*>(&m_CInterface);
		ffi_closure* closure = reinterpret_cast<ffi_closure*>(m_Closure);
		ffi_type** argumentTypes = reinterpret_cast<ffi_type**>(m_ArgumentTypes.data());
		ffi_type* returnType = reinterpret_cast<ffi_type*>(m_CInterface.m_ReturnType);
		ffi_abi abi = static_cast<ffi_abi>(m_CInterface.m_ABI);
		auto function = reinterpret_cast<void(*)(ffi_cif*, void*, void**, void*)>(KxFFI::CClosureCall);

		m_Status = static_cast<CStatus>(ffi_prep_cif(cif, abi, m_CInterface.m_ArgumentCount, returnType, argumentTypes));
		if (m_Status == CStatus::OK)
		{
			m_Status = static_cast<CStatus>(ffi_prep_closure_loc(closure, cif, function, this, m_Code));
			return m_Status == CStatus::OK;
		}
	}
	return false;
}

bool KxCFunction::AddParameter(TypeID type)
{
	if (type != TypeID::Void && m_CInterface.m_ArgumentCount + 1 < m_ArgumentTypes.size())
	{
		ffi_type* ffiType = GetFFIType(type);
		if (ffiType)
		{
			m_ArgumentTypes[m_CInterface.m_ArgumentCount] = reinterpret_cast<CType*>(ffiType);
			m_CInterface.m_ArgumentCount++;
			return true;
		}
	}
	return false;
}
TypeID KxCFunction::GetParameterType(size_t index) const
{
	return m_ArgumentTypes[index]->m_Type;
}

TypeID KxCFunction::GetReturnType() const
{
	return m_CInterface.m_ReturnType ? m_CInterface.m_ReturnType->m_Type : TypeID::Invalid;
}
void KxCFunction::SetReturnType(TypeID type)
{
	m_CInterface.m_ReturnType = reinterpret_cast<CType*>(GetFFIType(type));
}

KxCFunction& KxCFunction::operator=(KxCFunction&& other)
{
	// Copy
	m_CInterface = other.m_CInterface;
	m_ArgumentTypes = other.m_ArgumentTypes;
	m_Closure = other.m_Closure;
	m_Code = other.m_Code;
	m_Status = other.m_Status;

	// Clean other data
	other.m_CInterface = CInterface();
	other.m_ArgumentTypes.fill(nullptr);
	other.m_Closure = nullptr;
	other.m_Code = nullptr;
	other.m_Status = CStatus::OK;

	return *this;
}
