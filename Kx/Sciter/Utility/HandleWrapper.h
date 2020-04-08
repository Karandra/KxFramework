#pragma once

namespace KxFramework::Sciter
{
	template<class TObject, class THandle>
	class HandleWrapper
	{
		protected:
			THandle* m_Handle = nullptr;

		private:
			TObject& GetSelf()
			{
				return static_cast<TObject&>(*this);
			}
			const TObject& GetSelf() const
			{
				return static_cast<const TObject&>(*this);
			}

		protected:
			void Acquire(THandle* handle)
			{
				Release();
				if (GetSelf().DoAcquire(handle))
				{
					m_Handle = handle;
				}
			}
			void Release()
			{
				if (m_Handle)
				{
					GetSelf().DoRelease();
				}
				m_Handle = nullptr;
			}

			void CopyFrom(const HandleWrapper& other)
			{
				Release();
				Acquire(other.m_Handle);
			}
			void CopyFrom(THandle* handle)
			{
				Release();
				Acquire(handle);
			}
			void MoveFrom(HandleWrapper& other)
			{
				Release();
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;
			}
			
		protected:
			HandleWrapper() = default;
			HandleWrapper(THandle* handle)
			{
				Acquire(handle);
			}
			HandleWrapper(const HandleWrapper& other)
			{
				Acquire(other.m_Handle);
			}
			HandleWrapper(HandleWrapper&& other)
			{
				MoveFrom(other);
			}
			~HandleWrapper()
			{
				Release();
			}

		public:
			bool IsNull() const
			{
				return m_Handle == nullptr;
			}
			void MakeNull()
			{
				Release();
			}
			THandle* GetHandle() const
			{
				return m_Handle;
			}
		
			HandleWrapper& AttachHandle(THandle* handle)
			{
				if (IsNull())
				{
					m_Handle = handle;
				}
				return *this;
			}
			THandle* DetachHandle()
			{
				THandle* handle = m_Handle;
				m_Handle = nullptr;
				return handle;
			}

		public:
			bool operator==(const HandleWrapper& other) const
			{
				return m_Handle == other.m_Handle;
			}
			bool operator!=(const HandleWrapper& other) const
			{
				return !(*this == other);
			}

			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}
	};
}
