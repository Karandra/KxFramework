#pragma once
#include "kxf/IO/MemoryStream.h"
#include "kxf/EventSystem/EventID.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf::IPC::Private
{
	template<class TReturn = void, class TFunc, class... Args>
	TReturn InvokeProcedure(TFunc&& func, const EventID& procedureID, Args&&... arg)
	{
		// Serialize input parameters
		MemoryOutputStream parametersStream;
		std::initializer_list<uint64_t> list{Serialization::WriteObject(parametersStream, std::forward<Args>(arg)) ...};

		// Invoke actual function to perform the parameters transport
		if constexpr(std::is_void_v<TReturn>)
		{
			std::invoke(func, parametersStream, list.size(), false);
		}
		else
		{
			IInputStream& resultStream = std::invoke(func, parametersStream, list.size(), !std::is_void_v<TReturn>);
			static_assert(std::is_default_constructible_v<TReturn>, "TReturn must be default constructible");

			// Handle return value if non-void
			TReturn result;
			Serialization::ReadObject(resultStream, result);
			return result;
		}
	}

	template<class TReturn>
	TReturn GetProcedureResult(IInputStream& stream)
	{
		static_assert(std::is_default_constructible_v<TReturn>, "TReturn must be default constructible");

		TReturn result;
		Serialization::ReadObject(stream, result);
		return result;
	}

	template<class TReturn>
	uint64_t SetProcedureResult(IOutputStream& stream, const TReturn& result)
	{
		static_assert(!std::is_void<TReturn>, "TReturn must not be void");

		return Serialization::WriteObject(stream, result);
	}

	template<class... Args>
	class DeserializeParameters final
	{
		private:
		template<class TValue>
		TValue Read(IInputStream& stream, size_t index) const
		{
			static_assert(std::is_default_constructible_v<TValue>, "TValue must be default constructible");

			TValue value;
			Serialization::ReadObject(stream, value);
			return value;
		}

		public:
		template<size_t... t_Sequence>
		std::tuple<Args...> AsTuple(IInputStream& stream, std::index_sequence<t_Sequence...>) const
		{
			return std::make_tuple(Args{Read<Args>(stream, t_Sequence)}...);
		}
	};
}
