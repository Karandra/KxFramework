#pragma once
#include "kxf/IO/MemoryStream.h"
#include "kxf/EventSystem/EventID.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf::IPC::Private
{
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
		static_assert(!std::is_void_v<TReturn>, "TReturn must not be void");

		return Serialization::WriteObject(stream, result);
	}

	template<class TReturn = void, class TFunc, class... Args>
	TReturn InvokeProcedure(TFunc&& func, Args&&... arg)
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
			// Handle return value if non-void
			MemoryInputStream resultStream = std::invoke(func, parametersStream, list.size(), !std::is_void_v<TReturn>);
			return GetProcedureResult<TReturn>(resultStream);
		}
	}

	template<class... Args>
	class DeserializeParameters final
	{
		public:
			template<size_t... t_Sequence>
			std::tuple<Args...> AsTuple(IInputStream& stream, std::index_sequence<t_Sequence...>) const
			{
				static_assert((std::is_default_constructible_v<Args> && ...), "All types must be default constructible");

				// We are *required* to deserialize parameters in direct order (0, 1, 2, ...) and 'std::initializer_list'
				// does exactly that unlike construction of 'std::tuple<Args...>' which construct types from last to first
				// (..., 2, 1, 0). In either case we need to default construct the types to deserialize them, so it's not an
				// issue anyway.
				std::tuple<Args...> parameters;
				std::initializer_list<uint64_t> list{Serialization::ReadObject(stream, std::get<t_Sequence>(parameters)) ...};

				return parameters;
			}
	};
}
