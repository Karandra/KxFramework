#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"
#include <cstdint>
#include <tuple>
class KxCFunction;

namespace KxFFI
{
	enum class TypeID: uint16_t
	{
		Invalid = std::numeric_limits<uint16_t>::max(),
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
		Invalid = -1,

		#if _WIN64

		X64 = 1,
		SysV = X64,
		StdCall = X64,
		ThisCall = X64,
		FastCall = X64,
		CDecl = X64,
		Pascal = X64,
		Register = X64,
		Default = X64,

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

	// Layout of these types must be the same as their corresponding 'ffi_*' counterparts.
	struct CType
	{
		size_t m_Size = 0;
		uint16_t m_Alignemnt = 0;
		TypeID m_Type = TypeID::Invalid;
		CType** m_Elements = NULL;
	};
	struct CInterface
	{
		ABI m_ABI = ABI::Invalid;
		uint32_t m_ArgumentCount = 0;
		CType** m_ArgumentTypes = NULL;
		CType* m_ReturnType = NULL;
		uint32_t m_CodeSize = 0;
		uint32_t m_Flags = 0;
	};
	
	using CClosureFunction = void(*)(CInterface*, void*, void**, KxCFunction*);
	struct CClosure
	{
		uint8_t m_Trampoline[sizeof(void*) == 8 ? 29 : 52] = {0};
		CInterface* m_CInterface = NULL;
		CClosureFunction m_Function = NULL;
		KxCFunction* m_Context = NULL;
	};

	enum class CStatus
	{
		OK = 0,
		BadTypedef,
		BadABI
	};

	template<class T, class CStdT, class AliasT>
	inline constexpr bool IsTypeAlias = sizeof(CStdT) == sizeof(AliasT) && std::is_same_v<T, AliasT>;

	template<class T> constexpr TypeID GetTypeID()
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
		else if constexpr(std::is_same_v<T, int32_t> || IsTypeAlias<T, int32_t, long> || IsTypeAlias<T, int32_t, int>)
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
		else if constexpr(std::is_same_v<T, uint32_t> || IsTypeAlias<T, uint32_t, unsigned long> || IsTypeAlias<T, uint32_t, unsigned int>)
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
			static_assert(false, "This type is not supported");
		}
	}
	size_t GetTypeSize(TypeID type);
}

//////////////////////////////////////////////////////////////////////////
class KxCFunction
{
	public:
		using TypeID = KxFFI::TypeID;
		using ABI = KxFFI::ABI;

		using CType = KxFFI::CType;
		using CInterface = KxFFI::CInterface;
		using CClosure = KxFFI::CClosure;
		using CClosureFunction = KxFFI::CClosureFunction;
		using CStatus = KxFFI::CStatus;

		class ArgumentsWrapper
		{
			private:
				const KxCFunction& m_Function;
				void** m_Arguments = NULL;

			public:
				ArgumentsWrapper(const KxCFunction& function, void** arguments)
					:m_Function(function), m_Arguments(arguments)
				{
				}

			public:
				size_t GetCount() const
				{
					return m_Function.GetParametersCount();
				}

				template<size_t index, class T> T Get() const
				{
					return *reinterpret_cast<T*>(m_Arguments[index]);
				}
				template<size_t index, class T> void Get(T& value) const
				{
					value = *reinterpret_cast<T*>(m_Arguments[index]);
				}
				template<size_t index> TypeID GetType() const
				{
					return m_Function.GetParameterType(index);
				}
		};
		class ResultWrapper
		{
			private:
				void* m_ReturnValue = NULL;

			public:
				ResultWrapper(const KxCFunction& function, void* returnValue)
					:m_ReturnValue(returnValue)
				{
				}
			
			public:
				template<class T> void Set(const T& value)
				{
					*reinterpret_cast<T*>(m_ReturnValue) = value;
				}
		};

	private:
		CInterface m_CInterface;
		std::array<CType*, 16> m_ArgumentTypes;
		CClosure* m_Closure = NULL;
		void* m_Code = NULL;
		CStatus m_Status = CStatus::OK;

	private:
		static void CallExecute(CInterface* cif, void* returnValue, void** arguments, KxCFunction* context)
		{
			context->Execute(arguments, returnValue);
		};

	protected:
		virtual void Execute(void** arguments, void* returnValue) = 0;
		
		ArgumentsWrapper GetArgumentsWrapper(void** arguments) const
		{
			return ArgumentsWrapper(*this, arguments);
		}
		ResultWrapper GetResultWrapper(void* returnValue) const
		{
			return ResultWrapper(*this, returnValue);
		}

	public:
		KxCFunction() = default;
		KxCFunction(KxCFunction&& other)
		{
			*this = std::move(other);
		}
		KxCFunction(const KxCFunction&) = delete;
		virtual ~KxCFunction();

	public:
		bool IsOK() const;
		bool Create();

		const void* GetCode() const
		{
			return m_Code;
		}
		size_t GetCodeSize() const
		{
			return m_CInterface.m_CodeSize;
		}
		template<class T> T* GetFunctionPointer() const
		{
			return reinterpret_cast<T*>(m_Code);
		}

		size_t GetParametersCount() const
		{
			return m_CInterface.m_ArgumentCount;
		}
		bool HasParameters() const
		{
			return m_CInterface.m_ArgumentCount != 0;
		}
		bool AddParameter(TypeID type);
		TypeID GetParameterType(size_t index) const;

		TypeID GetReturnType() const;
		void SetReturnType(TypeID type);

		ABI GetABI() const
		{
			return m_CInterface.m_ABI;
		}
		void SetABI(ABI abi)
		{
			m_CInterface.m_ABI = abi;
		}

	public:
		KxCFunction& operator=(KxCFunction&& other);
		KxCFunction& operator=(const KxCFunction&) = delete;
};

//////////////////////////////////////////////////////////////////////////
template<class Signature, KxFFI::ABI t_ABI = KxFFI::ABI::Default> class KxCFunctor;

namespace KxFFI
{
	template<KxFFI::ABI t_ABI, class t_Ret, class... t_Types>
	class FunctorClass: protected KxCFunction
	{
		public:
			using ResultType = typename t_Ret;
			using DerivedType = typename KxCFunctor<t_Ret(t_Types...), t_ABI>;
			using FunctorType = typename std::function<t_Ret(t_Types...)>;
			using SignatureType = typename t_Ret(t_Types...);
			using FunctionPointerType = typename std::conditional_t<t_ABI == ABI::StdCall,
				t_Ret(__stdcall *)(t_Types...),
				t_Ret(__cdecl *)(t_Types...)
			>;

		protected:
			FunctorType m_Functor;

		private:
			void Init()
			{
				SetABI(t_ABI);
				SetReturnType(GetTypeID<ResultType>());
				SetParameters({GetTypeID<t_Types>()...});
				Create();
			}
			void SetParameters(const std::initializer_list<TypeID>& types)
			{
				for (TypeID type: types)
				{
					AddParameter(type);
				}
			}
			
			template<size_t... t_Sequence>
			std::tuple<t_Types...> ConvertToTuple(void** arguments, std::index_sequence<t_Sequence...>)
			{
				return std::make_tuple(t_Types {*reinterpret_cast<t_Types*>(arguments[t_Sequence])}...);
			}
			virtual void Execute(void** arguments, void* returnValue) override
			{
				const constexpr size_t argsCount = sizeof...(t_Types);
				std::tuple<t_Types...> args = ConvertToTuple(arguments, std::make_index_sequence<argsCount>());
				
				if constexpr(std::is_void_v<ResultType>)
				{
					std::apply(m_Functor, args);
				}
				else
				{
					*(reinterpret_cast<ResultType*>(returnValue)) = std::apply(m_Functor, args);
				}
			}

		public:
			FunctorClass()
			{
				Init();
			}
			FunctorClass(const FunctorType& functor)
				:m_Functor(functor)
			{
				Init();
			}
			FunctorClass(FunctorType&& functor)
				:m_Functor(std::move(functor))
			{
				Init();
			}

		public:
			const KxCFunction& GetBase() const
			{
				return *this;
			}
			FunctionPointerType GetFunction() const
			{
				return reinterpret_cast<FunctionPointerType>(GetCode());
			}

		public:
			ResultType operator()(t_Types&&... arg)
			{
				return m_Functor(std::forward<t_Types>(arg)...);
			}
			operator FunctionPointerType() const
			{
				return GetFunction();
			}

			operator bool() const
			{
				return m_Functor && IsOK();
			}
			bool operator!() const
			{
				return !m_Functor || !IsOK();
			}
	};

	template<KxFFI::ABI t_ABI, class T> struct FunctorClassWrapper;
	template<KxFFI::ABI t_ABI, class t_Ret, class... t_Types> struct FunctorClassWrapper<t_ABI, t_Ret(t_Types...)>
	{
		using ClassType = FunctorClass<t_ABI, t_Ret, t_Types...>;
	};
}

template<class Signature, KxFFI::ABI t_ABI>
class KxCFunctor: public KxFFI::FunctorClassWrapper<t_ABI, Signature>::ClassType
{
	public:
		using TypeID = KxFFI::TypeID;
		using ABI = KxFFI::ABI;

	private:
		using BaseType = typename KxFFI::FunctorClassWrapper<t_ABI, Signature>::ClassType;
		friend typename BaseType;

	public:
		using ResultType = typename BaseType::ResultType;
		using SignatureType = typename BaseType::SignatureType;
		using FunctionPointerType = typename BaseType::FunctionPointerType;

	public:
		KxCFunctor() = default;
		template<class Functor> KxCFunctor(const Functor& functor)
			:BaseType(functor)
		{
		}
		template<class Functor> KxCFunctor(Functor&& functor)
			:BaseType(std::move(functor))
		{
		}

	public:
		template<class Functor> KxCFunctor& operator=(const Functor& functor)
		{
			this->m_Functor = functor;
			return *this;
		}
		template<class Functor> KxCFunctor& operator=(Functor&& functor)
		{
			this->m_Functor = std::move(functor);
			return *this;
		}
};
