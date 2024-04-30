#pragma once
#include "Common.h"

namespace kxf::Threading
{
	struct TLSTraits final
	{
		using IndexType = uint32_t;

		static uint32_t GetInvalidIndex() noexcept;
		static uint32_t Initialize() noexcept;
		static bool Uninitialize(uint32_t index) noexcept;

		static void* GetValue(uint32_t index) noexcept;
		static bool SetValue(uint32_t index, void* ptr) noexcept;
	};

	struct FLSTraits final
	{
		using IndexType = uint32_t;
		using CallbackType = void(__stdcall)(void*);

		static uint32_t GetInvalidIndex() noexcept;
		static uint32_t Initialize() noexcept;
		static uint32_t Initialize(CallbackType* callback) noexcept;
		static bool Uninitialize(uint32_t index) noexcept;

		static void* GetValue(uint32_t index) noexcept;
		static bool SetValue(uint32_t index, void* ptr) noexcept;
	};
}

namespace kxf
{
	template<class TTraits_>
	class BasicThreadLocalSlot
	{
		public:
			using TTraits = TTraits_;

		protected:
			TTraits_::IndexType m_Index = TTraits_::GetInvalidIndex();

		public:
			BasicThreadLocalSlot() noexcept = default;
			BasicThreadLocalSlot(const BasicThreadLocalSlot&) = delete;
			BasicThreadLocalSlot(BasicThreadLocalSlot&& other) noexcept
			{
				*this = std::move(other);
			}
			~BasicThreadLocalSlot() noexcept
			{
				Uninitialize();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Index == TTraits_::GetInvalidIndex();
			}
			bool Initialize() noexcept
			{
				if (IsNull())
				{
					m_Index = TTraits_::Initialize();
					return !IsNull();
				}
				return true;
			}
			bool Uninitialize() noexcept
			{
				if (!IsNull())
				{
					bool result = TTraits_::Uninitialize(m_Index);
					m_Index = TTraits_::GetInvalidIndex();

					return result;
				}
				return false;
			}
			TTraits_::IndexType GetIndex() const noexcept
			{
				return m_Index;
			}

			void* GetValue() const noexcept
			{
				return TTraits_::GetValue(m_Index);
			}
			void SetValue(void* ptr) noexcept
			{
				TTraits_::SetValue(m_Index, ptr);
			}
			void* ExchangeValue(void* ptr) noexcept
			{
				void* old = GetValue();
				SetValue(ptr);

				return old;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			BasicThreadLocalSlot& operator=(const BasicThreadLocalSlot&) = delete;
			BasicThreadLocalSlot& operator=(BasicThreadLocalSlot&& other) noexcept
			{
				m_Index = other.m_Index;
				other.m_Index = TTraits_::GetInvalidIndex();

				return *this;
			}
	};
}

namespace kxf
{
	using ThreadLocalSlot = BasicThreadLocalSlot<Threading::TLSTraits>;

	class FiberLocalSlot final: public BasicThreadLocalSlot<Threading::FLSTraits>
	{
		public:
			using BasicThreadLocalSlot::BasicThreadLocalSlot;

		public:
			using BasicThreadLocalSlot::Initialize;
			bool Initialize(TTraits::CallbackType* callback) noexcept
			{
				if (IsNull())
				{
					m_Index = TTraits::Initialize(callback);
					return !IsNull();
				}
				return true;
			}
	};
}
