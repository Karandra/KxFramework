/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <tuple>

namespace KxSharedMemoryNS
{
	enum Protection: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Execute = 1 << 2,

		RW = Read|Write,
		RX = Read|Execute,
		RWX = Read|Write|Execute,
	};

	class KX_API Buffer
	{
		public:
			using Protection = KxSharedMemoryNS::Protection;

		public:
			static bool AllocateRegion(HANDLE& handle, void*& buffer, size_t size, uint32_t protection = Protection::RW, const wchar_t* name = nullptr);
			static bool OpenRegion(HANDLE& handle, void*& buffer, const wchar_t* name, size_t size, uint32_t protection = Protection::RW);
			static void FreeRegion(HANDLE handle, void* buffer);

		private:
			HANDLE m_Handle = INVALID_HANDLE_VALUE;
			void* m_Buffer = nullptr;
			size_t m_Size = 0;
			Protection m_Protection = Protection::None;

		private:
			void MakeNull()
			{
				m_Handle = INVALID_HANDLE_VALUE;
				m_Buffer = nullptr;
				m_Size = 0;
				m_Protection = Protection::None;
			}

		public:
			Buffer()
			{
			}
			Buffer(size_t size, uint32_t protection, const wxString& name)
			{
				Allocate(size, protection, name);
			}
			Buffer(size_t size, uint32_t protection = Protection::RW, const wchar_t* name = nullptr)
			{
				Allocate(size, protection, name);
			}
			Buffer(const Buffer& other)
			{
				*this = other;
			}
			Buffer(Buffer&& other)
			{
				*this = std::move(other);
			}
			~Buffer()
			{
				FreeIfNeeded();
			}

		public:
			bool Open(const wchar_t* name, size_t size, uint32_t protection = Protection::RW)
			{
				FreeIfNeeded();
				if (OpenRegion(m_Handle, m_Buffer, name, size, protection))
				{
					m_Size = size;
					m_Protection = static_cast<Protection>(protection);
					return true;
				}
				else
				{
					MakeNull();
					return false;
				}
			}
			bool Open(const wxString& name, size_t size, uint32_t protection = Protection::RW)
			{
				return Open(name.wc_str(), size, protection);
			}

			bool Allocate(size_t size, uint32_t protection, const wxString& name)
			{
				return Allocate(size, protection, name.IsEmpty() ? nullptr : name.wc_str());
			}
			bool Allocate(size_t size, uint32_t protection, const wchar_t* name = nullptr)
			{
				FreeIfNeeded();
				if (AllocateRegion(m_Handle, m_Buffer, size, protection, name))
				{
					m_Size = size;
					m_Protection = static_cast<Protection>(protection);
					return true;
				}
				else
				{
					MakeNull();
					return false;
				}
			}
			
			void Free()
			{
				FreeRegion(m_Handle, m_Buffer);
				MakeNull();
			}
			bool FreeIfNeeded()
			{
				if (IsOK())
				{
					Free();
					return true;
				}
				return false;
			}
			void ZeroBuffer()
			{
				::RtlSecureZeroMemory(m_Buffer, m_Size);
			}

			void Acquire(HANDLE handle, void* data, size_t size, uint32_t protection)
			{
				FreeIfNeeded();
				
				m_Handle = handle;
				m_Buffer = data;
				m_Size = size;
				m_Protection = static_cast<Protection>(protection);
			}
			HANDLE Release()
			{
				const HANDLE handle = m_Handle;
				MakeNull();
				return handle;
			}

		public:
			bool IsOK() const
			{
				return m_Handle != INVALID_HANDLE_VALUE && m_Buffer != nullptr && m_Size != 0;
			}

			uint32_t GetProtection() const
			{
				return m_Protection;
			}
			bool IsReadable() const
			{
				return m_Protection & Protection::Read;
			}
			bool IsWriteable() const
			{
				return m_Protection & Protection::Write;
			}
			bool IsExecutable() const
			{
				return m_Protection & Protection::Execute;
			}

			size_t GetSize() const
			{
				return m_Size;
			}
			const void* GetBuffer() const
			{
				return m_Buffer;
			}
			void* GetBuffer()
			{
				return m_Buffer;
			}

			bool CreateFrom(const Buffer& other)
			{
				FreeIfNeeded();
				if (other.IsOK())
				{
					// Allocate new unnamed buffer with same parameters
					return Allocate(other.m_Size, other.m_Protection);
				}
				return false;
			}

		public:
			template<class T> void GetAs(T& value) const
			{
				static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				if (IsOK())
				{
					value = *reinterpret_cast<T*>(m_Buffer);
				}
			}
			template<class T> T GetAs() const
			{
				T value;
				GetAs(value);
				return value;
			}
			
			template<> void GetAs(wxString& value) const
			{
				if (IsOK())
				{
					value = wxString(reinterpret_cast<const wxChar*>(m_Buffer), m_Size);
				}
			}
			template<> wxString GetAs() const
			{
				wxString value;
				GetAs(value);
				return value;
			}

			void WriteData(const void* data, size_t dataSize) const
			{
				std::memcpy(m_Buffer, data, std::min(dataSize, m_Size));
			}
			template<class T> void WriteData(const T& value) const
			{
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				WriteData(&value, sizeof(T));
			}
			template<> void WriteData(const wxString& value) const
			{
				WriteData(value.wc_str(), value.length() * sizeof(wxChar) + sizeof(wxChar));
			}

		public:
			explicit operator bool() const
			{
				return IsOK();
			}
			bool operator!() const
			{
				return !IsOK();
			}

			Buffer& operator=(Buffer&& other)
			{
				FreeIfNeeded();
				m_Handle = other.m_Handle;
				m_Buffer = other.m_Buffer;
				m_Size = other.m_Size;
				m_Protection = other.m_Protection;

				other.MakeNull();
				return *this;
			}
			Buffer& operator=(const Buffer& other)
			{
				if (CreateFrom(other))
				{
					// If succeed, copy other's data if possible
					if (IsWriteable() && other.IsReadable())
					{
						std::memcpy(m_Buffer, other.m_Buffer, m_Size);
					}
				}
				return *this;
			}
	};

	template<class T, Protection t_Protection = Protection::RW> class TypedBuffer
	{
		public:
			using Protection = KxSharedMemoryNS::Protection;
			using ObjectType = T;

		private:
			Buffer m_Object;
			bool m_IsReference = false;

		private:
			template<class... Args> void Construct(Args&&... args)
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
				if (!m_IsReference && m_Object.IsOK())
				{
					Destroy();
				}
			}

		public:
			TypedBuffer()
			{
			}
			template<class... Args> TypedBuffer(const wxString& name, Args&&... args)
				:m_Object(sizeof(ObjectType), t_Protection, name)
			{
				if (m_Object.IsOK())
				{
					Construct(std::forward<Args>(args)...);
				}
			}
			TypedBuffer(const TypedBuffer& other)
			{
				*this = other;
			}
			TypedBuffer(TypedBuffer&& other)
			{
				*this = std::move(other);
			}
			~TypedBuffer()
			{
				DestroyIfNeeded();
			}

		public:
			bool IsReference() const
			{
				return m_IsReference;
			}
			bool Open(const wxString& name)
			{
				DestroyIfNeeded();
				m_IsReference = m_Object.Open(name, sizeof(ObjectType), t_Protection);
				return m_IsReference;
			}

		public:
			operator bool() const
			{
				return m_Object.IsOK();
			}
			bool operator!() const
			{
				return !m_Object.IsOK();
			}
			
			template<class... Args> TypedBuffer& operator()(Args&&... args)
			{
				if (m_Object.IsOK())
				{
					Destroy();
					Construct(std::forward<Args>(args)...);
				}
				return *this;
			}
			ObjectType* operator->() const
			{
				return static_cast<ObjectType*>(const_cast<void*>(m_Object.GetBuffer()));
			}

			const ObjectType* operator&() const
			{
				return static_cast<const ObjectType*>(m_Object.GetBuffer());
			}
			ObjectType* operator&()
			{
				return static_cast<ObjectType*>(m_Object.GetBuffer());
			}
	
			const ObjectType& operator*() const
			{
				return *static_cast<const ObjectType*>(m_Object.GetBuffer());
			}
			ObjectType& operator*()
			{
				return *static_cast<ObjectType*>(m_Object.GetBuffer());
			}
	
		public:
			TypedBuffer& operator=(TypedBuffer&& other)
			{
				static_assert(std::is_move_assignable_v<ObjectType>, "This can not be moved");

				DestroyIfNeeded();
				m_Object = std::move(other.m_Object);
				return *this;
			}
			TypedBuffer& operator=(const TypedBuffer& other)
			{
				static_assert(std::is_copy_assignable_v<ObjectType>, "This can not be copied");

				// Destroy old object and reallocate with new parameters
				DestroyIfNeeded();
				m_Object.CreateFrom(other);
				
				// Invoke class copy operator
				ObjectType* object = static_cast<ObjectType*>(m_Object.GetBuffer());
				object->operator=(*other);

				return *this;
			}
	};
	
	template<class T> class TypedBufferRef
	{
		public:
			using Protection = KxSharedMemoryNS::Protection;
			using ObjectType = T;
			using BufferType = TypedBuffer<T>;

		private:
			ObjectType* m_Object = nullptr;

		public:
			TypedBufferRef()
			{
			}
			TypedBufferRef(BufferType& buffer)
				:m_Object(&buffer)
			{
			}

		public:
			operator bool() const
			{
				return m_Object != nullptr;
			}
			bool operator!() const
			{
				return m_Object == nullptr;
			}

			ObjectType* operator->() const
			{
				return m_Object;
			}
	
			const ObjectType* operator&() const
			{
				return m_Object;
			}
			ObjectType* operator&()
			{
				return m_Object;
			}
	
			const ObjectType& operator*() const
			{
				return *m_Object;
			}
			ObjectType& operator*()
			{
				return *m_Object;
			}
	};

	template<class T> class Allocator
	{
		public:
			using value_type = T;
			using size_type = size_t;
			using difference_type = ptrdiff_t;

		private:
			class AllocationInfo
			{
				public:
					HANDLE m_Handle = INVALID_HANDLE_VALUE;
					void* m_Buffer = nullptr;
					size_t m_Size = 0;

				public:
					AllocationInfo(size_t size)
						:m_Size(size)
					{
					}
			
				public:
					bool IsOK(value_type* userBuffer, size_t userSize) const
					{
						// Field 'm_Buffer' contains original address
						// Field 'm_Size' contains original size (including 'AllocationInfo' size)
						return m_Handle != INVALID_HANDLE_VALUE &&
							reinterpret_cast<uint8_t*>(userBuffer) - sizeof(*this) == reinterpret_cast<uint8_t*>(m_Buffer) &&
							userSize + sizeof(*this) == m_Size;
					}
			};

		public:
			value_type* allocate(size_t count) const
			{
				// Allocate requested size + size for allocation info struct
				const size_t size = sizeof(AllocationInfo) + sizeof(value_type) * count;
				if (size != 0)
				{
					AllocationInfo allocInfo(size);
					if (Buffer::AllocateRegion(allocInfo.m_Handle, allocInfo.m_Buffer, size, Protection::RW, nullptr))
					{
						// Write allocation info at the begging of allocated space
						*reinterpret_cast<AllocationInfo*>(allocInfo.m_Buffer) = allocInfo;

						// Return address
						return reinterpret_cast<value_type*>(reinterpret_cast<uint8_t*>(allocInfo.m_Buffer) + sizeof(AllocationInfo));
					}
				}
				return nullptr;
			}
			void deallocate(value_type* buffer, size_t count) const
			{
				const size_t size = sizeof(value_type) * count;
				AllocationInfo* allocInfo = reinterpret_cast<AllocationInfo*>(reinterpret_cast<uint8_t*>(buffer) - sizeof(AllocationInfo));

				if (allocInfo->IsOK(buffer, size))
				{
					Buffer::FreeRegion(allocInfo->m_Handle, allocInfo->m_Buffer);
				}
			}
	};
	template<class T1, class T2> bool operator==(const Allocator<T1>& lhs, const Allocator<T2>& rhs)
	{
		return true;
	}
	template<class T1, class T2> bool operator!=(const Allocator<T1>& lhs, const Allocator<T2>& rhs)
	{
		return false;
	}
};

using KxSharedMemoryBuffer = KxSharedMemoryNS::Buffer;

template<class T, KxSharedMemoryNS::Protection t_Protection = KxSharedMemoryNS::Protection::RW>
using KxSharedMemory = KxSharedMemoryNS::TypedBuffer<T, t_Protection>;

template<class T> using KxSharedMemoryRef = KxSharedMemoryNS::TypedBufferRef<T>;
template<class T> using KxSharedMemoryAllocator = KxSharedMemoryNS::Allocator<T>;
