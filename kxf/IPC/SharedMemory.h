#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include <tuple>

namespace kxf
{
	class IInputStream;
	class IOutputStream;
}

namespace kxf::IPC
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
				if (m_Handle = IPC::OpenSharedMemoryRegion(m_Buffer, name, size, protection))
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
				if (m_Handle = IPC::AllocateSharedMemoryRegion(m_Buffer, size, protection, name))
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
				IPC::FreeSharedMemoryRegion(m_Handle, m_Buffer);
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
			std::unique_ptr<IInputStream> GetInputStream() const;
			std::unique_ptr<IOutputStream> GetOutputStream();

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
