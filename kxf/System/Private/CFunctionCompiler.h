#pragma once
#include "../Common.h"
#include <limits>

namespace kxf::FFI
{
	class CFunctionCompiler;

	enum class TypeID: uint16_t
	{
		None = std::numeric_limits<uint16_t>::max(),
		Void = 0,

		Float32 = 2,
		Float64 = 3,

		UInt8 = 5,
		Int8 = 6,
		UInt16 = 7,
		Int16 = 8,
		UInt32 = 9,
		Int32 = 10,
		UInt64 = 11,
		Int64 = 12,

		Pointer = 14
	};
	enum class ABI: int32_t
	{
		None = -1,

		#if _WIN64

		Win64 = 1,
		GNU64 = 2,
		Default = Win64,

		SysV = Default,
		StdCall = Default,
		ThisCall = Default,
		FastCall = Default,
		CDecl = Default,
		Pascal = Default,
		Register = Default,

		#else

		SysV = 1,
		StdCall = 2,
		ThisCall = 3,
		FastCall = 4,
		CDecl = 5,
		Pascal = 6,
		Register = 7,
		Default = CDecl,

		#endif
	};
}

namespace kxf::FFI::Private
{
	// Layout of these types must be the same as their corresponding 'ffi_*' counterparts.
	enum class CStatus
	{
		Unknown = -1,

		Success = 0,
		BadTypedef,
		BadABI
	};

	struct CType final
	{
		size_t m_Size = 0;
		uint16_t m_Alignemnt = 0;
		TypeID m_Type = TypeID::None;
		CType** m_Elements = nullptr;
	};
	struct CInterface final
	{
		using OnCall = void(*)(CInterface*, void*, void**, CFunctionCompiler*);

		ABI m_ABI = ABI::None;
		uint32_t m_ArgumentCount = 0;
		const CType** m_ArgumentTypes = nullptr;
		const CType* m_ReturnType = nullptr;
		uint32_t m_CodeSize = 0;
		uint32_t m_Flags = 0;
	};
	struct CClosure;

	template<class T>
	constexpr TypeID GetTypeID() noexcept
	{
		if constexpr(std::is_pointer_v<T>)
		{
			return TypeID::Pointer;
		}
		else if constexpr(std::is_void_v<T>)
		{
			return TypeID::Void;
		}
		else if constexpr(std::is_same_v<T, int8_t>)
		{
			return TypeID::Int8;
		}
		else if constexpr(std::is_same_v<T, int16_t>)
		{
			return TypeID::Int16;
		}
		else if constexpr(std::is_same_v<T, int32_t>)
		{
			return TypeID::Int32;
		}
		else if constexpr(std::is_same_v<T, int64_t>)
		{
			return TypeID::Int64;
		}
		else if constexpr(std::is_same_v<T, uint8_t>)
		{
			return TypeID::UInt8;
		}
		else if constexpr(std::is_same_v<T, uint16_t>)
		{
			return TypeID::UInt16;
		}
		else if constexpr(std::is_same_v<T, uint32_t>)
		{
			return TypeID::UInt32;
		}
		else if constexpr(std::is_same_v<T, uint64_t>)
		{
			return TypeID::UInt64;
		}
		else if constexpr(std::is_same_v<T, float>)
		{
			return TypeID::Float32;
		}
		else if constexpr(std::is_same_v<T, double>)
		{
			return TypeID::Float64;
		}
		else
		{
			static_assert(sizeof(T*) == 0, "this type is not supported");
		}
	}
}
