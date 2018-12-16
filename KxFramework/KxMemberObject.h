/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include <cstdint>
#include <memory>
#include <utility>

template<class t_ObjectType, size_t t_Size, size_t t_SizeX64 = t_Size>
class KxMemberObject
{
	public:
		using ObjectType = t_ObjectType;

	private:
		uint8_t m_Buffer[sizeof(void*) == 8 ? t_SizeX64 : t_Size];

	public:
		template<class... Args> KxMemberObject(Args&&... args)
		{
			static_assert((sizeof(void*) == 8 ? t_SizeX64 : t_Size) == sizeof(ObjectType), "KxMemberObject: buffer size does not match object size");

			try
			{
				new(m_Buffer) ObjectType(std::forward<Args>(args)...);
			}
			catch (...)
			{
				throw;
			}
		}
		~KxMemberObject()
		{
			GetAs<ObjectType>()->~ObjectType();
		}

	public:
		const void* data() const noexcept
		{
			return m_Buffer;
		}
		void* data() noexcept
		{
			return m_Buffer;
		}
		constexpr size_t size() const noexcept
		{
			return std::size(m_Buffer);
		}

		template<class T> const T* GetAs() const noexcept
		{
			return reinterpret_cast<const T*>(m_Buffer);
		}
		template<class T> T* GetAs() noexcept
		{
			return reinterpret_cast<T*>(m_Buffer);
		}

	public:
		operator const ObjectType*() const
		{
			return GetAs<ObjectType>();
		}
		operator ObjectType*()
		{
			return GetAs<ObjectType>();
		}

		const ObjectType* operator->() const
		{
			return GetAs<ObjectType>();
		}
		ObjectType* operator->()
		{
			return GetAs<ObjectType>();
		}

		const ObjectType** operator&() const
		{
			return static_cast<const ObjectType**>(&m_Buffer);
		}
		ObjectType** operator&()
		{
			return static_cast<ObjectType**>(&m_Buffer);
		}

		const ObjectType& operator*() const
		{
			return *GetAs<ObjectType>();
		}
		ObjectType& operator*()
		{
			return *GetAs<ObjectType>();
		}
};
