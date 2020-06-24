#pragma once
#include "Common.h"
#include "ErrorCode.h"
#include "kxf/General/String.h"
#include "kxf/General/NativeUUID.h"
#include "Private/COM.h"
struct IUnknown;
struct _GUID;

namespace kxf
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
	enum class ClassContext
	{
		InprocServer = 0 << 0,
		InprocHandler = 0 << 1,
		LocalServer = 0 << 2,
		RemoteServer = 0 << 3,
		EnableCodeDownload = 0 << 4,
		NoCodeDownload = 0 << 5,
		NoCustomMarshal = 0 << 6,
		NoFailureLog = 0 << 7,
		DisableAAA = 0 << 8,
		EnableAAA = 0 << 9,
		ActivateAAAAsIU = 0 << 10,
		FromDefaultContext = 0 << 11,
		Activate32BitServer = 0 << 12,
		Activate64BitServer = 0 << 13,
		ActivateARM32Server = 0 << 14,
		EnableCloaking = 0 << 15,
		AppContainer = 0 << 16
	};

	Kx_DeclareFlagSet(COMInitFlag);
	Kx_DeclareFlagSet(ClassContext);
}

namespace kxf::COM
{
	void* AllocateMemory(size_t size) noexcept;
	void* ReallocateMemory(void* address, size_t size) noexcept;
	void FreeMemory(void* address) noexcept;

	::_GUID ToGUID(const NativeUUID& uuid) noexcept;
	NativeUUID FromGUID(const ::_GUID& guid) noexcept;
}

namespace kxf::COM
{
	template<class T>
	NativeUUID UUIDOf() noexcept
	{
		#ifdef _MSC_EXTENSIONS
		return FromGUID(__uuidof(T));
		#else
		#error Microsoft language extensions support required
		#endif
	}

	inline NativeUUID ToUUID(const NativeUUID& uuid) noexcept
	{
		return uuid;
	}
	inline NativeUUID ToUUID(const ::_GUID& guid) noexcept
	{
		return FromGUID(guid);
	}
}

namespace kxf::COM
{
	HResult CreateInstance(const NativeUUID& classID, ClassContext classContext, const NativeUUID& iid, void** result, IUnknown* outer = nullptr) noexcept;
	inline HResult CreateInstance(const ::_GUID& classID, ClassContext classContext, const ::_GUID& iid, void** result, IUnknown* outer = nullptr) noexcept
	{
		return CreateInstance(FromGUID(classID), classContext, FromGUID(iid), result, outer);
	}

	template<class AnyID, class T>
	HResult CreateInstance(const AnyID& classID, ClassContext classContext, T** result, IUnknown* outer = nullptr) noexcept
	{
		return CreateInstance(ToUUID(classID), classContext, UUIDOf<T>(), reinterpret_cast<void**>(result), outer);
	}
}

namespace kxf
{
	class KX_API COMInitGuard final
	{
		private:
			HResult m_Status = HResult::Pending();

		private:
			void DoInitialize(COMThreadingModel threadingModel, FlagSet<COMInitFlag> flags) noexcept;
			void DoUninitialize() noexcept;

		public:
			COMInitGuard(COMThreadingModel threadingModel, FlagSet<COMInitFlag> flags = {}) noexcept
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
			HResult GetStatus() const noexcept
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
			HResult m_Status = HResult::Pending();

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
			HResult GetStatus() const noexcept
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

namespace kxf
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
					COM::FreeMemory(reinterpret_cast<void*>(const_cast<std::remove_cv_t<TValue>*>(m_Value)));
					m_Value = ptr;
				}
		};
	}

	template<class T>
	class COMPtr: public COM::Private::BasicPtr<T, COM::Private::PtrTraits<T>>
	{
		private:
			using Base = COM::Private::BasicPtr<T, COM::Private::PtrTraits<T>>;

		public:
			COMPtr(T* ptr = nullptr) noexcept
				:Base(ptr)
			{
			}
			COMPtr(COMPtr&& other) noexcept
				:Base(std::move(other))
			{
			}
			COMPtr(const COMPtr& other) noexcept
				:Base(other.m_Value)
			{
				if (this->m_Value)
				{
					this->m_Value->AddRef();
				}
			}

		public:
			COMPtr& operator=(COMPtr&& other) noexcept
			{
				this->Reset(other.Detach());
				return *this;
			}
			COMPtr& operator=(const COMPtr& other) noexcept
			{
				this->Reset(other.m_Value);
				if (this->m_Value)
				{
					this->m_Value->AddRef();
				}
				return *this;
			}
	};

	template<class T>
	using COMMemoryPtr = COM::Private::BasicPtr<T, COM::Private::MemoryPtrTraits<T>>;
}

namespace kxf::COM
{
	template<class T, class TRefCount = uint32_t>
	class RefCount final
	{
		private:
			std::atomic<TRefCount> m_RefCount = 0;
			T* m_Object = nullptr;

		public:
			RefCount(T& object)
				:m_Object(&object)
			{
				static_assert(std::is_integral_v<TRefCount>);
			}

		public:
			TRefCount AddRef()
			{
				return ++m_RefCount;
			}
			TRefCount Release()
			{
				const TRefCount newCount = --m_RefCount;
				if (newCount == 0)
				{
					delete m_Object;
				}
				return newCount;
			}
	};
}

namespace kxf::COM
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
