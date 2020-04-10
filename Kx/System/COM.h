#pragma once
#include "Common.h"
#include "ErrorCode.h"

namespace KxFramework
{
	enum class COMThreadingModel
	{
		Apartment,
		Concurrent
	};
}

namespace KxFramework
{
	class KX_API COMInitGuard final
	{
		private:
			ErrorCode m_Status;

		private:
			void DoInitialize(COMThreadingModel threadingModel);
			void DoUninitialize();

		public:
			COMInitGuard(COMThreadingModel threadingModel = COMThreadingModel::Apartment)
			{
				DoInitialize(threadingModel);
			}
			COMInitGuard(COMInitGuard&&) = default;
			COMInitGuard(const COMInitGuard&) = delete;
			~COMInitGuard()
			{
				DoUninitialize();
			}

		public:
			ErrorCode GetStatus() const
			{
				return m_Status;
			}
			bool IsInitialized() const
			{
				return m_Status.IsSuccess();
			}
			void Uninitialize()
			{
				DoUninitialize();
			}

			explicit operator bool() const
			{
				return IsInitialized();
			}
			bool operator!() const
			{
				return !IsInitialized();
			}

		public:
			COMInitGuard& operator=(COMInitGuard&&) = default;
			COMInitGuard& operator=(const COMInitGuard&) = delete;
	};

	class KX_API OLEInitGuard final
	{
		private:
			ErrorCode m_Status;

		private:
			void DoInitialize();
			void DoUninitialize();

		public:
			OLEInitGuard()
			{
				DoInitialize();
			}
			OLEInitGuard(const COMInitGuard&) = delete;
			OLEInitGuard(OLEInitGuard&&) = default;
			~OLEInitGuard()
			{
				DoUninitialize();
			}

		public:
			ErrorCode GetStatus() const
			{
				return m_Status;
			}
			bool IsInitialized() const
			{
				return m_Status.IsSuccess();
			}
			void Uninitialize()
			{
				DoUninitialize();
			}

			explicit operator bool() const
			{
				return IsInitialized();
			}
			bool operator!() const
			{
				return !IsInitialized();
			}

		public:
			OLEInitGuard& operator=(const OLEInitGuard&) = delete;
			OLEInitGuard& operator=(OLEInitGuard&& other) = default;
	};
}

struct IUnknown;
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
			COMPtr(TObject* ptr = nullptr)
				:m_Object(ptr)
			{
				static_assert(std::is_base_of_v<IUnknown, T>, "class T is not derived from IUnknown");
			}
			COMPtr(COMPtr&& other)
			{
				*this = std::move(other);
			}
			~COMPtr()
			{
				Reset();
			}

		public:
			void Reset(TObject* newPtr = nullptr)
			{
				if (m_Object)
				{
					m_Object->Release();
				}
				m_Object = newPtr;
			}
			TObject* Detach()
			{
				TObject* ptr = m_Object;
				m_Object = nullptr;
				return ptr;
			}

			TObject* Get() const
			{
				return m_Object;
			}
			void** GetAddress() const
			{
				return reinterpret_cast<void**>(const_cast<TObject**>(&m_Object));
			}
			
			operator const TObject*() const
			{
				return m_Object;
			}
			operator TObject* ()
			{
				return m_Object;
			}

			const TObject& operator*() const
			{
				return *m_Object;
			}
			TObject& operator*()
			{
				return *m_Object;
			}
			TObject** operator&()
			{
				return &m_Object;
			}

			TObject* operator->() const
			{
				return m_Object;
			}
			TObject* operator->()
			{
				return m_Object;
			}

		public:
			explicit operator bool() const
			{
				return m_Object != nullptr;
			}
			bool operator!() const
			{
				return m_Object == nullptr;
			}

			bool operator==(const COMPtr& other) const
			{
				return m_Object == other.m_Object;
			}
			bool operator==(const TObject* other) const
			{
				return m_Object == other;
			}
			bool operator==(const TObject& other) const
			{
				return m_Object == &other;
			}
			bool operator!=(const COMPtr& other) const
			{
				return !(*this == other);
			}
			bool operator!=(const TObject* other) const
			{
				return !(*this == other);
			}
			bool operator!=(const TObject& other) const
			{
				return !(*this == other);
			}

			COMPtr& operator=(const COMPtr&) = delete;
			COMPtr& operator=(COMPtr&& other)
			{
				Reset(other.Detach());
				return *this;
			}
			COMPtr& operator=(TObject* ptr)
			{
				Reset(ptr);
				return *this;
			}
	};
}
