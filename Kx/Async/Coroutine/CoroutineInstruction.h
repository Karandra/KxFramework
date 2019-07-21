#pragma once
#include "Kx/Async/Common.h"
#include "CoroutineImpl.h"

class KX_API KxCoroutine;
namespace Kx::Async
{
	class KX_API BaseCoroutine;

	enum class InstructionType
	{
		Delay,
		Continue,
		Terminate,
	};

	template<class T> void AssertStateType()
	{
		static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
	}
}

class KX_API KxYieldInstruction
{
	friend class Kx::Async::BaseCoroutine;
	friend class KxCoroutine;

	private:
		using InstructionType = Kx::Async::InstructionType;

	private:
		InstructionType m_Type = InstructionType::Continue;
		wxTimeSpan m_Delay;
		std::optional<intptr_t> m_NextState;

	protected:
		KxYieldInstruction(InstructionType instruction) noexcept
			:m_Type(instruction)
		{
		}
		template<class T> KxYieldInstruction(InstructionType type, const T& nextState) noexcept
			:m_Type(type), m_NextState(static_cast<intptr_t>(nextState))
		{
			Kx::Async::AssertStateType<T>();
		}

	public:
		InstructionType GetType() const noexcept
		{
			return m_Type;
		}
		wxTimeSpan GetDelay() const
		{
			return m_Delay;
		}
		
		template<class T = intptr_t> std::optional<T> GetNextState() const noexcept
		{
			Kx::Async::AssertStateType<T>();
			if (m_NextState)
			{
				return static_cast<T>(*m_NextState);
			}
			else
			{
				return std::nullopt;
			}
		}
};
