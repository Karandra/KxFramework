#include "stdafx.h"
#include "CFunction.h"
#include <ffi.h>

namespace kxf::FFI::Private
{
	constexpr const ffi_type* GetFFIType(TypeID type) noexcept
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
	constexpr size_t GetFFITypeSize(TypeID type) noexcept
	{
		return GetFFIType(type)->size;
	}

	struct CClosure final
	{
		uint8_t m_Trampoline[FFI_TRAMPOLINE_SIZE] = {};
		CInterface* m_CInterface = nullptr;
		CInterface::OnCall m_Function = nullptr;
		CFunctionCompiler* m_Context = nullptr;
	};
}

namespace kxf::FFI
{
	bool CFunctionCompiler::IsCreated() const noexcept
	{
		using namespace Private;

		// Layout
		static_assert(sizeof(CInterface) == sizeof(ffi_cif), "[CFunctionCompiler] CInterface invalid layout");
		static_assert(sizeof(CType) == sizeof(ffi_type), "[CFunctionCompiler] CType invalid layout");
		static_assert(sizeof(CStatus) == sizeof(ffi_status), "[CFunctionCompiler] CStatus invalid layout");
		static_assert(sizeof(CClosure) == sizeof(CClosure), "[CFunctionCompiler] CClosure invalid layout");

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

		return m_Closure && m_Code && m_CInterface.m_ReturnType && m_Status == CStatus::Success;
	}
	bool CFunctionCompiler::Create() noexcept
	{
		using namespace Private;

		if (!IsCreated())
		{
			m_Closure = reinterpret_cast<CClosure*>(ffi_closure_alloc(sizeof(ffi_closure), &m_Code));
			if (m_Closure)
			{
				ffi_cif* cif = reinterpret_cast<ffi_cif*>(&m_CInterface);
				ffi_closure* closure = reinterpret_cast<ffi_closure*>(m_Closure);
				ffi_type** argumentTypes = const_cast<ffi_type**>(reinterpret_cast<const ffi_type**>(m_ArgumentTypes.data()));
				ffi_type* returnType = const_cast<ffi_type*>(reinterpret_cast<const ffi_type*>(m_CInterface.m_ReturnType));
				ffi_abi abi = static_cast<ffi_abi>(m_CInterface.m_ABI);

				m_Status = static_cast<CStatus>(ffi_prep_cif(cif, abi, m_CInterface.m_ArgumentCount, returnType, argumentTypes));
				if (m_Status == CStatus::Success)
				{
					m_Status = static_cast<CStatus>(ffi_prep_closure_loc(closure, cif, [](ffi_cif* cif, void* returnValue, void** arguments, void* context)
					{
						reinterpret_cast<CFunctionCompiler*>(context)->Execute(arguments, returnValue);
					}, this, m_Code));
					return m_Status == CStatus::Success;
				}
			}
		}
		return false;
	}
	void CFunctionCompiler::Destroy() noexcept
	{
		using namespace Private;

		if (m_Closure)
		{
			ffi_closure_free(m_Closure);
			m_Closure = nullptr;
		}

		m_CInterface = {};
		m_ArgumentTypes.fill(nullptr);
		m_Closure = nullptr;
		m_Code = nullptr;
		m_Status = CStatus::Success;
	}

	bool CFunctionCompiler::AddParameter(TypeID type) noexcept
	{
		if (SetParameter(m_CInterface.m_ArgumentCount + 1, type))
		{
			m_CInterface.m_ArgumentCount++;
			return true;
		}
		return false;
	}
	bool CFunctionCompiler::SetParameter(size_t index, TypeID type) noexcept
	{
		using namespace Private;

		if (type != TypeID::Void && index < m_ArgumentTypes.size())
		{
			if (const ffi_type* ffiType = GetFFIType(type))
			{
				m_ArgumentTypes[index] = reinterpret_cast<const CType*>(ffiType);
				return true;
			}
		}
		return false;
	}
	TypeID CFunctionCompiler::GetParameterType(size_t index) const noexcept
	{
		return m_ArgumentTypes[index]->m_Type;
	}

	TypeID CFunctionCompiler::GetReturnType() const noexcept
	{
		return m_CInterface.m_ReturnType ? m_CInterface.m_ReturnType->m_Type : TypeID::None;
	}
	void CFunctionCompiler::SetReturnType(TypeID type) noexcept
	{
		using namespace Private;

		m_CInterface.m_ReturnType = reinterpret_cast<const CType*>(GetFFIType(type));
	}

	CFunctionCompiler& CFunctionCompiler::operator=(CFunctionCompiler&& other)  noexcept
	{
		m_CInterface = std::move(other.m_CInterface);
		m_ArgumentTypes = std::move(other.m_ArgumentTypes);
		m_Closure = std::move(other.m_Closure);
		m_Code = std::move(other.m_Code);
		m_Status = std::move(other.m_Status);

		other.m_Closure = nullptr;
		other.Destroy();

		return *this;
	}
}
