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
	void* AllocateGlobalSharedMemoryRegion(void*& buffer, size_t size, FlagSet<MemoryProtection> protection, const String& name = {}) noexcept;
	void* AllocateLocalSharedMemoryRegion(void*& buffer, size_t size, FlagSet<MemoryProtection> protection, const String& name = {}) noexcept;

	void* OpenGloablSharedMemoryRegion(void*& buffer, const String& name, size_t size, FlagSet<MemoryProtection> protection) noexcept;
	void* OpenLocalSharedMemoryRegion(void*& buffer, const String& name, size_t size, FlagSet<MemoryProtection> protection) noexcept;

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

			template<class TFunc>
			bool DoOpen(const String& name, size_t size, FlagSet<MemoryProtection> protection, TFunc&& func) noexcept
			{
				Free();
				if (m_Handle = std::invoke(func, m_Buffer, name, size, protection))
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

			template<class TFunc>
			bool DoAllocate(size_t size, FlagSet<MemoryProtection> protection, const String& name, TFunc&& func) noexcept
			{
				Free();
				if (m_Handle = std::invoke(func, m_Buffer, size, protection, name))
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

		public:
			SharedMemoryBuffer() noexcept = default;
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

			bool OpenGlobal(const String& name, size_t size, FlagSet<MemoryProtection> protection) noexcept
			{
				return DoOpen(name, size, protection, IPC::OpenGloablSharedMemoryRegion);
			}
			bool OpenLocal(const String& name, size_t size, FlagSet<MemoryProtection> protection) noexcept
			{
				return DoOpen(name, size, protection, IPC::OpenLocalSharedMemoryRegion);
			}

			bool AllocateGlobal(size_t size, FlagSet<MemoryProtection> protection, const String& name = {}) noexcept
			{
				return DoAllocate(size, protection, name, IPC::AllocateGlobalSharedMemoryRegion);
			}
			bool AllocateLocal(size_t size, FlagSet<MemoryProtection> protection, const String& name = {}) noexcept
			{
				return DoAllocate(size, protection, name, IPC::AllocateLocalSharedMemoryRegion);
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
