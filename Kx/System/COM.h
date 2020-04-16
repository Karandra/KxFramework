#pragma once
#include "Common.h"
#include "ErrorCode.h"
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
	template<class T>
	class COMPtr final
	{
		public:
			using TObject = T;

		private:
			TObject* m_Object = nullptr;

		public:
			COMPtr(TObject* ptr = nullptr) noexcept
				:m_Object(ptr)
			{
				static_assert(std::is_base_of_v<IUnknown, T>, "class T is not derived from IUnknown");
			}
			COMPtr(COMPtr&& other) noexcept
			{
				*this = std::move(other);
			}
			~COMPtr() noexcept
			{
				Reset();
			}

		public:
			void Reset(TObject* newPtr = nullptr) noexcept
			{
				if (m_Object)
				{
					m_Object->Release();
				}
				m_Object = newPtr;
			}
			TObject* Detach() noexcept
			{
				TObject* ptr = m_Object;
				m_Object = nullptr;
				return ptr;
			}

			TObject* Get() const noexcept
			{
				return m_Object;
			}
			void** GetAddress() const noexcept
			{
				return reinterpret_cast<void**>(const_cast<TObject**>(&m_Object));
			}
			
			operator const TObject*() const noexcept
			{
				return m_Object;
			}
			operator TObject*() noexcept
			{
				return m_Object;
			}

			const TObject& operator*() const noexcept
			{
				return *m_Object;
			}
			TObject& operator*() noexcept
			{
				return *m_Object;
			}
			TObject** operator&() noexcept
			{
				return &m_Object;
			}

			TObject* operator->() const noexcept
			{
				return m_Object;
			}
			TObject* operator->() noexcept
			{
				return m_Object;
			}

		public:
			explicit operator bool() const noexcept
			{
				return m_Object != nullptr;
			}
			bool operator!() const noexcept
			{
				return m_Object == nullptr;
			}

			bool operator==(const COMPtr& other) const noexcept
			{
				return m_Object == other.m_Object;
			}
			bool operator==(const TObject* other) const noexcept
			{
				return m_Object == other;
			}
			bool operator==(const TObject& other) const noexcept
			{
				return m_Object == &other;
			}
			bool operator!=(const COMPtr& other) const noexcept
			{
				return !(*this == other);
			}
			bool operator!=(const TObject* other) const noexcept
			{
				return !(*this == other);
			}
			bool operator!=(const TObject& other) const noexcept
			{
				return !(*this == other);
			}

			COMPtr& operator=(const COMPtr&) = delete;
			COMPtr& operator=(COMPtr&& other) noexcept
			{
				Reset(other.Detach());
				return *this;
			}
			COMPtr& operator=(TObject* ptr) noexcept
			{
				Reset(ptr);
				return *this;
			}
	};
}
