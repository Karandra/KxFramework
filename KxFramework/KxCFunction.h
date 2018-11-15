#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"
#include <cstdint>
#include <tuple>

namespace KxFFI
{
	class FunctionData;
	enum class Type
	{
		Invalid = -1,

		Void,
		Pointer,
		Int8, Int16, Int32, Int64,
		UInt8, UInt16, UInt32, UInt64,
		Float32, Float64,
	};
	enum class ABI
	{
		Invalid = -1,
		Default = 0,

		CDECL_ABI,
		STDCALL_ABI,
		THISCALL_ABI,

		X64_ABI,
		VECTORCALL_ABI
	};

	template<Type> struct CType {};
	template<> struct CType<Type::Invalid> {};
	template<> struct CType<Type::Void> { using T = void; };
	template<> struct CType<Type::Pointer> { using T = void*; };
	template<> struct CType<Type::Int8> { using T = int8_t; };
	template<> struct CType<Type::Int16> { using T = int16_t; };
	template<> struct CType<Type::Int32> { using T = int32_t; };
	template<> struct CType<Type::Int64> { using T = int64_t; };
	template<> struct CType<Type::UInt16> { using T = uint16_t; };
	template<> struct CType<Type::UInt32> { using T = uint32_t; };
	template<> struct CType<Type::UInt64> { using T = uint64_t; };
	template<> struct CType<Type::Float32> { using T = float; };
	template<> struct CType<Type::Float64> { using T = double; };

	template<class T, class CStdT, class AliasT>
	inline constexpr bool IsTypeAlias = sizeof(CStdT) == sizeof(AliasT) && std::is_same_v<T, AliasT>;

	template<class T> constexpr Type CRType()
	{
		if constexpr(std::is_pointer_v<T>)
		{
			return Type::Pointer;
		}
		else if constexpr(std::is_void_v<T>)
		{
			return Type::Void;
		}

		else if constexpr(std::is_same_v<T, int8_t>)
		{
			return Type::Int8;
		}
		else if constexpr(std::is_same_v<T, int16_t>)
		{
			return Type::Int16;
		}
		else if constexpr(std::is_same_v<T, int32_t> || IsTypeAlias<T, int32_t, long> || IsTypeAlias<T, int32_t, int>)
		{
			return Type::Int32;
		}
		else if constexpr(std::is_same_v<T, int64_t>)
		{
			return Type::Int64;
		}

		else if constexpr(std::is_same_v<T, uint8_t>)
		{
			return Type::UInt8;
		}
		else if constexpr(std::is_same_v<T, uint16_t>)
		{
			return Type::UInt16;
		}
		else if constexpr(std::is_same_v<T, uint32_t> || IsTypeAlias<T, uint32_t, unsigned long> || IsTypeAlias<T, uint32_t, unsigned int>)
		{
			return Type::UInt32;
		}
		else if constexpr(std::is_same_v<T, uint64_t>)
		{
			return Type::UInt64;
		}

		else if constexpr(std::is_same_v<T, float>)
		{
			return Type::Float32;
		}
		else if constexpr(std::is_same_v<T, double>)
		{
			return Type::Float64;
		}

		else
		{
			static_assert(false, "This type is not supported");
		}
	}

	size_t GetMaxParameterCount();
	size_t GetTypeSize(Type type);
}

//////////////////////////////////////////////////////////////////////////
class KxCFunction
{
	public:
		using FunctionData = KxFFI::FunctionData;
		using Type = KxFFI::Type;
		using ABI = KxFFI::ABI;

		class Arguments
		{
			private:
				const KxCFunction& m_Function;
				void** m_Arguments = NULL;

			public:
				Arguments(const KxCFunction& function, void** arguments)
					:m_Function(function), m_Arguments(arguments)
				{
				}

			public:
				template<size_t index, class T> T GetAs() const
				{
					return *reinterpret_cast<T*>(m_Arguments[index]);
				}
				
				template<size_t index, class T = void> T GetPointer() const
				{
					static_assert(std::is_pointer_v<T>, "[Arguments::GetPointer()] Only pointer types is allowed");
					return GetAs<index, T>();
				}
				template<size_t index> int8_t GetInt8() const
				{
					return GetAs<index, int8_t>();
				}
				template<size_t index> int16_t GetInt16() const
				{
					return GetAs<index, int16_t>();
				}
				template<size_t index> int32_t GetInt32() const
				{
					return GetAs<index, int32_t>();
				}
				template<size_t index> int64_t GetInt64() const
				{
					return GetAs<index, int64_t>();
				}
				template<size_t index> uint8_t GetUInt8() const
				{
					return GetAs<index, uint8_t>();
				}
				template<size_t index> uint16_t GetUInt16() const
				{
					return GetAs<index, uint16_t>();
				}
				template<size_t index> uint32_t GetUInt32() const
				{
					return GetAs<index, uint32_t>();
				}
				template<size_t index> uint64_t GetUInt64() const
				{
					return GetAs<index, uint64_t>();
				}
				template<size_t index> float GetFloat32() const
				{
					return GetAs<index, float>();
				}
				template<size_t index> double GetFloat64() const
				{
					return GetAs<index, double>();
				}
		};

	private:
		std::unique_ptr<FunctionData> m_FunctionData;

	protected:
		virtual void Execute(void** arguments, void* returnValue) = 0;

	public:
		KxCFunction();
		KxCFunction(KxCFunction&& other);
		KxCFunction(const KxCFunction&) = delete;
		virtual ~KxCFunction();

	public:
		bool Create();
		bool IsOK() const;

		const void* GetCode() const;
		size_t GetCodeSize() const;
		template<class T> T* GetFunctionPointer() const
		{
			return reinterpret_cast<T*>(GetCode());
		}

		size_t GetParametersCount() const;
		bool HasParameters() const;
		bool AddParameter(Type type);
		Type GetParameterType(size_t index) const;

		Type GetReturnType() const;
		void SetReturnType(Type type);

		ABI GetABI() const;
		void SetABI(ABI abi);

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
			using FinalType = typename KxCFunctor<t_Ret(t_Types...), t_ABI>;
			using FunctorType = typename std::function<t_Ret(t_Types...)>;
			using SignatureType = typename t_Ret(t_Types...);

			using FunctionPointerType = typename std::conditional_t<t_ABI == ABI::STDCALL_ABI,
				t_Ret(__stdcall *)(t_Types...),
				t_Ret(__cdecl *)(t_Types...)
			>;

		private:
			FinalType& m_FinalObject;

		protected:
			FunctorType m_Functor;

		private:
			void Init()
			{
				SetABI(t_ABI);
				SetReturnType(CRType<ResultType>());
				SetParameters({CRType<t_Types>()...});
				Create();
			}
			void SetParameters(const std::initializer_list<Type>& types)
			{
				for (Type type: types)
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
			FunctorClass(FinalType& function)
				:m_FinalObject(function)
			{
				Init();
			}
			FunctorClass(FinalType& function, const FunctorType& functor)
				:m_FinalObject(function), m_Functor(functor)
			{
				Init();
			}
			FunctorClass(FinalType& function, FunctorType&& functor)
				:m_FinalObject(function), m_Functor(std::move(functor))
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
		using Type = KxFFI::Type;
		using ABI = KxFFI::ABI;

	private:
		using FullBaseType = typename KxFFI::FunctorClassWrapper<t_ABI, Signature>::ClassType;
		friend typename FullBaseType;

	public:
		using ResultType = typename FullBaseType::ResultType;
		using SignatureType = typename FullBaseType::SignatureType;
		using FunctionPointerType = typename FullBaseType::FunctionPointerType;

	public:
		KxCFunctor()
			:FullBaseType(*this)
		{
		}
		template<class Functor> KxCFunctor(const Functor& functor)
			:FullBaseType(*this, functor)
		{
		}
		template<class Functor> KxCFunctor(Functor&& functor)
			:FullBaseType(*this, std::move(functor))
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
