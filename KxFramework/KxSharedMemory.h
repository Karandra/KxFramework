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

	class Buffer
	{
		public:
			using Protection = KxSharedMemoryNS::Protection;

		private:
			HANDLE m_Handle = INVALID_HANDLE_VALUE;
			void* m_Buffer = NULL;
			size_t m_Size = 0;
			Protection m_Protection = Protection::None;

		private:
			void FreeIfNeeded()
			{
				if (IsOK())
				{
					Free();
				}
			}
			void MakeNull()
			{
				m_Handle = INVALID_HANDLE_VALUE;
				m_Buffer = NULL;
				m_Size = 0;
				m_Protection = Protection::None;
			}

		public:
			Buffer()
			{
			}
			Buffer(size_t size, uint32_t protection = Protection::RW, const wxString& name = wxEmptyString)
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
			bool Open(const wxString& name, size_t size, uint32_t protection = Protection::RW);
			bool Allocate(size_t size, uint32_t protection, const wxString& name = wxEmptyString);
			void Free();

		public:
			bool IsOK() const
			{
				return m_Handle != INVALID_HANDLE_VALUE && m_Buffer != NULL && m_Size != 0;
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
	
			bool CreateFrom(const Buffer& other);

		public:
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
				if (m_Object.IsOK())
				{
					Destroy();
				}
			}

		public:
			template<class... Args> TypedBuffer(const wxString& name = wxEmptyString)
				:m_Object()
			{
				m_Object.Open(name, sizeof(ObjectType), t_Protection);
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
			ObjectType* m_Object = NULL;

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
				return m_Object != NULL;
			}
			bool operator!() const
			{
				return m_Object == NULL;
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
};

using KxSharedMemoryBuffer = KxSharedMemoryNS::Buffer;

template<class T, KxSharedMemoryNS::Protection t_Protection = KxSharedMemoryNS::Protection::RW>
using KxSharedMemory = KxSharedMemoryNS::TypedBuffer<T, t_Protection>;

template<class T> using KxSharedMemoryRef = KxSharedMemoryNS::TypedBufferRef<T>;
