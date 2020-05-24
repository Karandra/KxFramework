#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include <tuple>

namespace kxf::System
{
	void* AllocateSharedMemoryRegion(void*& buffer, size_t size, FlagSet<MemoryProtection> protection, const wchar_t* name = nullptr) noexcept;
	void* OpenSharedMemoryRegion(void*& buffer, const wchar_t* name, size_t size, FlagSet<MemoryProtection> protection) noexcept;
	void FreeSharedMemoryRegion(void* handle, void* buffer) noexcept;
}

namespace kxf
{
	class KX_API SharedMemoryBuffer final
	{
		private:
			void* m_Handle = nullptr;
			void* m_Buffer = nullptr;
			size_t m_Size = 0;
			FlagSet<MemoryProtection> m_Protection;

		private:
			void MakeNull() noexcept
			{
				m_Handle = nullptr;
				m_Buffer = nullptr;
				m_Size = 0;
				m_Protection = MemoryProtection::None;
			}

		public:
			SharedMemoryBuffer() noexcept = default;
			SharedMemoryBuffer(size_t size, FlagSet<MemoryProtection> protection, const String& name)
			{
				Allocate(size, protection, name);
			}
			SharedMemoryBuffer(size_t size, FlagSet<MemoryProtection> protection, const wchar_t* name = nullptr)
			{
				Allocate(size, protection, name);
			}
			SharedMemoryBuffer(SharedMemoryBuffer&& other) noexcept
			{
				*this = std::move(other);
			}
			SharedMemoryBuffer(const SharedMemoryBuffer&) = delete;
			~SharedMemoryBuffer() noexcept
			{
				Free();
			}

		public:
			bool IsNull() const noexcept
			{
				return !m_Handle || !m_Buffer || m_Size == 0;
			}

			bool Open(const wchar_t* name, size_t size, FlagSet<MemoryProtection> protection) noexcept
			{
				Free();
				if (m_Handle = System::OpenSharedMemoryRegion(m_Buffer, name, size, protection))
				{
					m_Size = size;
					m_Protection = protection;
					return true;
				}
				else
				{
					MakeNull();
					return false;
				}
			}
			bool Open(const String& name, size_t size, FlagSet<MemoryProtection> protection) noexcept
			{
				return Open(name.wc_str(), size, protection);
			}

			bool Allocate(size_t size, FlagSet<MemoryProtection> protection, const String& name) noexcept
			{
				return Allocate(size, protection, name.IsEmpty() ? nullptr : name.wc_str());
			}
			bool Allocate(size_t size, FlagSet<MemoryProtection> protection, const wchar_t* name = nullptr) noexcept
			{
				Free();
				if (m_Handle = System::AllocateSharedMemoryRegion(m_Buffer, size, protection, name))
				{
					m_Size = size;
					m_Protection = protection;
					return true;
				}
				else
				{
					MakeNull();
					return false;
				}
			}

			SharedMemoryBuffer Clone(const wchar_t* name = nullptr) const noexcept
			{
				// Allocate new buffer with the same parameters
				SharedMemoryBuffer buffer;
				if (buffer.Allocate(m_Size, m_Protection, name))
				{
					std::memcpy(buffer.m_Buffer, m_Buffer, m_Size);
				}
				return buffer;
			}
			SharedMemoryBuffer Clone(const String& name) const noexcept
			{
				return Clone(name.wc_str());
			}
			
			void Free() noexcept
			{
				System::FreeSharedMemoryRegion(m_Handle, m_Buffer);
				MakeNull();
			}
			void ZeroBuffer() noexcept;

			void Acquire(void* handle, void* data, size_t size, FlagSet<MemoryProtection> protection) noexcept
			{
				Free();
				
				m_Handle = handle;
				m_Buffer = data;
				m_Size = size;
				m_Protection = protection;
			}
			void* Release() noexcept
			{
				void* handle = m_Handle;
				MakeNull();
				return handle;
			}

			FlagSet<MemoryProtection> GetProtection() const noexcept
			{
				return m_Protection;
			}
			bool IsReadable() const noexcept
			{
				return m_Protection & MemoryProtection::Read;
			}
			bool IsWriteable() const noexcept
			{
				return m_Protection & MemoryProtection::Write;
			}
			bool IsExecutable() const noexcept
			{
				return m_Protection & MemoryProtection::Execute;
			}

			size_t GetSize() const noexcept
			{
				return m_Size;
			}
			const void* GetBuffer() const noexcept
			{
				return m_Buffer;
			}
			void* GetBuffer() noexcept
			{
				return m_Buffer;
			}

		public:
			template<class T>
			void GetAs(T& value) const noexcept
			{
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				value = *reinterpret_cast<T*>(m_Buffer);
			}

			template<class T>
			T GetAs() const noexcept
			{
				static_assert(std::is_default_constructible_v<T>, "T must be default constructible");

				T value;
				GetAs(value);
				return value;
			}
			
			template<>
			void GetAs(String& value) const
			{
				const wxChar* data = reinterpret_cast<const wxChar*>(m_Buffer);
				value = String(data, wcsnlen_s(data, m_Size));
			}

			template<>
			String GetAs() const
			{
				String value;
				GetAs(value);
				return value;
			}

			size_t WriteData(const void* data, size_t dataSize) const noexcept
			{
				size_t copied = std::min(dataSize, m_Size);
				std::memcpy(m_Buffer, data, copied);

				return copied;
			}

			template<class T>
			size_t WriteData(const T& value) const noexcept
			{
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				return WriteData(&value, sizeof(T));
			}
			
			template<>
			size_t WriteData(const String& value) const
			{
				return WriteData(value.wx_str(), value.length() * sizeof(wxChar) + sizeof(wxChar));
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

			SharedMemoryBuffer& operator=(SharedMemoryBuffer&& other) noexcept
			{
				if (this != &other)
				{
					Acquire(other.m_Handle, other.m_Buffer, other.m_Size, other.m_Protection);
					other.Release();
				}
				return *this;
			}
			SharedMemoryBuffer& operator=(const SharedMemoryBuffer&) = delete;
	};
}

namespace kxf
{
	template<class T, MemoryProtection t_Protection = MemoryProtection::RW>
	class TypedSharedMemory final
	{
		public:
			using ObjectType = T;

		private:
			SharedMemoryBuffer m_Object;
			bool m_IsReference = false;

		private:
			template<class... Args>
			void Construct(Args&&... args)
			{
				try
				{
					new (m_Object.GetBuffer()) ObjectType(std::forward<Args>(args)...);
				}
				catch (...)
				{
					m_Object.Free();
					throw;
				}
			}

			void Destroy()
			{
				static_cast<ObjectType*>(m_Object.GetBuffer())->~ObjectType();
			}
			void DestroyIfNeeded()
			{
				if (!m_IsReference && m_Object)
				{
					Destroy();
				}
			}

		public:
			TypedSharedMemory() noexcept = default;
			
			template<class... Args>
			TypedSharedMemory(const String& name, Args&&... args)
				:m_Object(sizeof(ObjectType), t_Protection, name)
			{
				if (m_Object.IsNull())
				{
					Construct(std::forward<Args>(args)...);
				}
			}
			
			TypedSharedMemory(TypedSharedMemory&& other) noexcept
			{
				*this = std::move(other);
			}
			TypedSharedMemory(const TypedSharedMemory&) = delete;
			~TypedSharedMemory()
			{
				DestroyIfNeeded();
			}

		public:
			bool IsReference() const noexcept
			{
				return m_IsReference;
			}
			bool Open(const String& name)
			{
				DestroyIfNeeded();
				m_IsReference = m_Object.Open(name, sizeof(ObjectType), t_Protection);
				return m_IsReference;
			}

		public:
			explicit operator bool() const
			{
				return !m_Object.IsNull();
			}
			bool operator!() const
			{
				return m_Object.IsNull();
			}
			
			template<class... Args>
			TypedSharedMemory& operator()(Args&&... args)
			{
				if (!m_Object.IsNull())
				{
					Destroy();
					Construct(std::forward<Args>(args)...);
				}
				return *this;
			}

			ObjectType* operator->() const noexcept
			{
				return static_cast<ObjectType*>(const_cast<void*>(m_Object.GetBuffer()));
			}

			const ObjectType* operator&() const noexcept
			{
				return static_cast<const ObjectType*>(m_Object.GetBuffer());
			}
			ObjectType* operator&() noexcept
			{
				return static_cast<ObjectType*>(m_Object.GetBuffer());
			}
	
			const ObjectType& operator*() const noexcept
			{
				return *static_cast<const ObjectType*>(m_Object.GetBuffer());
			}
			ObjectType& operator*() noexcept
			{
				return *static_cast<ObjectType*>(m_Object.GetBuffer());
			}
	
		public:
			TypedSharedMemory& operator=(TypedSharedMemory&& other) noexcept
			{
				DestroyIfNeeded();

				m_Object = std::move(other.m_Object);
				m_IsReference = other.m_IsReference;
				other.m_IsReference = false;

				return *this;
			}
			TypedSharedMemory& operator=(const TypedSharedMemory&) = delete;
	};
}

namespace kxf
{
	template<class T>
	class TypedSharedMemoryRef final
	{
		public:
			using ObjectType = T;
			using BufferType = TypedSharedMemory<T>;

		private:
			ObjectType* m_Object = nullptr;

		public:
			TypedSharedMemoryRef() noexcept = default;
			TypedSharedMemoryRef(BufferType& buffer) noexcept
				:m_Object(&buffer)
			{
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

			ObjectType* operator->() const noexcept
			{
				return m_Object;
			}
			
			const ObjectType* operator&() const noexcept
			{
				return m_Object;
			}
			ObjectType* operator&() noexcept
			{
				return m_Object;
			}
			
			const ObjectType& operator*() const noexcept
			{
				return *m_Object;
			}
			ObjectType& operator*() noexcept
			{
				return *m_Object;
			}
	};
};
