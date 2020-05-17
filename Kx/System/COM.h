#pragma once
#include "Common.h"
#include "ErrorCode.h"
#include "Kx/General/String.h"
#include "Private/COM.h"
struct IUnknown;

namespace KxFramework
{
	enum class COMThreadingModel
	{
		Apartment,
		Concurrent
	};
	enum class COMInitFlag
	{
		None = 0,

		DisableOLE1DDE = 1 << 0,
		SppedOverMemory = 1 << 1
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(COMInitFlag);
	}
}

namespace KxFramework::COM
{
	void* AllocateMemory(size_t size) noexcept;
	void* ReallocateMemory(void* address, size_t size) noexcept;
	void FreeMemory(void* address) noexcept;
}

namespace KxFramework
{
	class KX_API COMInitGuard final
	{
		private:
			ErrorCode m_Status;

		private:
			void DoInitialize(COMThreadingModel threadingModel, COMInitFlag flags) noexcept;
			void DoUninitialize() noexcept;

		public:
			COMInitGuard(COMThreadingModel threadingModel, COMInitFlag flags = COMInitFlag::None) noexcept
			{
				DoInitialize(threadingModel, flags);
			}
			COMInitGuard(COMInitGuard&&) noexcept = default;
			COMInitGuard(const COMInitGuard&) = delete;
			~COMInitGuard()
			{
				DoUninitialize();
			}

		public:
			ErrorCode GetStatus() const noexcept
			{
				return m_Status;
			}
			bool IsInitialized() const noexcept
			{
				return m_Status.IsSuccess();
			}
			void Uninitialize() noexcept
			{
				DoUninitialize();
			}

			explicit operator bool() const noexcept
			{
				return IsInitialized();
			}
			bool operator!() const noexcept
			{
				return !IsInitialized();
			}

		public:
			COMInitGuard& operator=(COMInitGuard&&) noexcept = default;
			COMInitGuard& operator=(const COMInitGuard&) = delete;
	};

	class KX_API OLEInitGuard final
	{
		private:
			ErrorCode m_Status;

		private:
			void DoInitialize() noexcept;
			void DoUninitialize() noexcept;

		public:
			OLEInitGuard() noexcept
			{
				DoInitialize();
			}
			OLEInitGuard(const COMInitGuard&) = delete;
			OLEInitGuard(OLEInitGuard&&) noexcept = default;
			~OLEInitGuard() noexcept
			{
				DoUninitialize();
			}

		public:
			ErrorCode GetStatus() const noexcept
			{
				return m_Status;
			}
			bool IsInitialized() const noexcept
			{
				return m_Status.IsSuccess();
			}
			void Uninitialize() noexcept
			{
				DoUninitialize();
			}

			explicit operator bool() const noexcept
			{
				return IsInitialized();
			}
			bool operator!() const noexcept
			{
				return !IsInitialized();
			}

		public:
			OLEInitGuard& operator=(const OLEInitGuard&) = delete;
			OLEInitGuard& operator=(OLEInitGuard&& other) noexcept = default;
	};
}

namespace KxFramework
{
	namespace COM::Private
	{
		template<class TValue>
		class PtrTraits final
		{
			private:
				TValue*& m_Value = nullptr;

			public:
				PtrTraits(TValue*& ptr)
					:m_Value(ptr)
				{
				}

			public:
				void Reset(TValue* ptr = nullptr) noexcept
				{
					if (m_Value)
					{
						m_Value->Release();
					}
					m_Value = ptr;
				}
		};

		template<class TValue>
		class MemoryPtrTraits final
		{
			private:
				TValue*& m_Value = nullptr;

			public:
				MemoryPtrTraits(TValue*& ptr)
					:m_Value(ptr)
				{
				}

			public:
				void Reset(TValue* ptr = nullptr) noexcept
				{
					COM::FreeMemory(m_Value);
					m_Value = ptr;
				}
		};
	}

	template<class T>
	using COMPtr = COM::Private::BasicPtr<T, COM::Private::PtrTraits<T>>;

	template<class T>
	using COMMemoryPtr = COM::Private::BasicPtr<T, COM::Private::MemoryPtrTraits<T>>;
}

namespace KxFramework::COM
{
	template<class TChar>
	COMMemoryPtr<TChar> AllocateRawString(std::basic_string_view<TChar> value) noexcept
	{
		const size_t size = value.length() * sizeof(TChar);
		if (size != 0)
		{
			if (TChar* buffer = reinterpret_cast<TChar*>(AllocateMemory(size + sizeof(TChar))))
			{
				std::memcpy(buffer, value.data(), size);
				buffer[size] = 0;

				return buffer;
			}
		}
		else
		{
			if (TChar* buffer = reinterpret_cast<TChar*>(AllocateMemory(1)))
			{
				buffer[0] = 0;
				return buffer;
			}
		}
		return nullptr;
	}

	template<class T>
	auto AllocateRawString(const T& value) noexcept
	{
		return AllocateRawString(StringViewOf<T>(value));
	}

	template<class TObject>
	COMMemoryPtr<TObject> AllocateObject() noexcept
	{
		return reinterpret_cast<TObject*>(AllocateMemory(sizeof(TObject)));
	}

	template<class TObject>
	COMMemoryPtr<TObject> AllocateObject(TObject object) noexcept(std::is_nothrow_move_assignable_v<TObject>)
	{
		auto buffer = AllocateObject<TObject>();
		if (buffer)
		{
			*buffer = std::move(object);
		}
		return buffer;
	}
}
