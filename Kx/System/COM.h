#pragma once
#include "Common.h"
#include "ErrorCode.h"
#include "Kx/General/String.h"
#include "Kx/General/NativeUUID.h"
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

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(COMInitFlag);
		Kx_EnumClass_AllowEverything(ClassContext);
	}
}

namespace KxFramework::COM
{
	void* AllocateMemory(size_t size) noexcept;
	void* ReallocateMemory(void* address, size_t size) noexcept;
	void FreeMemory(void* address) noexcept;

	constexpr ::GUID ToGUID(const NativeUUID& uuid) noexcept
	{
		::GUID guid = {};
		guid.Data1 = uuid.Data1;
		guid.Data2 = uuid.Data2;
		guid.Data3 = uuid.Data3;
		for (size_t i = 0; i < std::size(guid.Data4); i++)
		{
			guid.Data4[i] = uuid.Data4[i];
		}

		return guid;
	}
	constexpr NativeUUID FromGUID(const ::GUID& guid) noexcept
	{
		NativeUUID uuid;
		uuid.Data1 = guid.Data1;
		uuid.Data2 = guid.Data2;
		uuid.Data3 = guid.Data3;
		for (size_t i = 0; i < std::size(uuid.Data4); i++)
		{
			uuid.Data4[i] = guid.Data4[i];
		}

		return uuid;
	}
}

namespace KxFramework::COM
{
	template<class T>
	constexpr NativeUUID UUIDOf() noexcept
	{
		return FromGUID(__uuidof(T));
	}

	inline constexpr NativeUUID ToUUID(const NativeUUID& uuid) noexcept
	{
		return uuid;
	}
	inline constexpr NativeUUID ToUUID(const ::GUID& guid) noexcept
	{
		return FromGUID(guid);
	}
}

namespace KxFramework::COM
{
	HResult CreateInstance(const NativeUUID& classID, ClassContext classContext, const NativeUUID& iid, void** result, IUnknown* outer = nullptr) noexcept;
	inline HResult CreateInstance(const ::GUID& classID, ClassContext classContext, const ::GUID& iid, void** result, IUnknown* outer = nullptr) noexcept
	{
		return CreateInstance(FromGUID(classID), classContext, FromGUID(iid), result, outer);
	}

	template<class AnyID, class T>
	HResult CreateInstance(const AnyID& classID, ClassContext classContext, T** result, IUnknown* outer = nullptr) noexcept
	{
		return CreateInstance(ToUUID(classID), classContext, UUIDOf<T>(), reinterpret_cast<void**>(result), outer);
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
