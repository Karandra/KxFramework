#pragma once
#include "Common.h"
#include <variant>

namespace KxFramework
{
	class KX_API IExtraData
	{
		public:
			virtual ~IExtraData() = default;
	};
}

namespace KxFramework
{
	namespace Internal::ExtraDataContainer
	{
		template<class T>
		constexpr void AssertUntypedStorageType()
		{
			using Tx = std::remove_reference_t<T>;
			static_assert(sizeof(Tx) <= sizeof(void*) && (std::is_trivially_copyable_v<Tx>), "invalid type for untyped storage");
		}

		template<class T = void*>
		T GetExtraData(void* data)
		{
			AssertUntypedStorageType<T>();

			return reinterpret_cast<T>(data);
		}

		template<class T>
		void* SetExtraData(T&& data)
		{
			AssertUntypedStorageType<T>();

			return reinterpret_cast<void*>(data);
		}
	}

	class KX_API TrivialExtraDataContainer
	{
		private:
			void* m_Data = nullptr;

		public:
			virtual ~TrivialExtraDataContainer() = default;

		public:
			template<class T = void*>
			T GetExtraData() const
			{
				return Internal::ExtraDataContainer::GetExtraData<T>(m_Data);
			}

			template<class T>
			void SetExtraData(T&& data)
			{
				m_Data = Internal::ExtraDataContainer::SetExtraData(std::forward<T>(data));
			}
	};

	class KX_API ExtraDataContainer
	{
		public:
			enum class Type: size_t
			{
				Untyped,
				Typed
			};

		private:
			std::variant<void*, std::unique_ptr<IExtraData>> m_Data;

		public:
			ExtraDataContainer() = default;
			ExtraDataContainer(const ExtraDataContainer&) = delete;
			ExtraDataContainer(ExtraDataContainer&&) = default;
			virtual ~ExtraDataContainer() = default;

		public:
			Type GetType() const
			{
				return static_cast<Type>(m_Data.index()) == Type::Typed ? Type::Typed : Type::Untyped;
			}

			// Untyped data
			template<class T = void*>
			T GetExtraData() const
			{
				if (GetType() == Type::Untyped)
				{
					void* data = std::get<static_cast<size_t>(Type::Untyped)>(m_Data);
					return Internal::ExtraDataContainer::GetExtraData<T>(data);
				}
				return T{};
			}

			template<class T>
			void SetExtraData(T&& data)
			{
				m_Data = Internal::ExtraDataContainer::SetExtraData(std::forward<T>(data));
			}
			
			// Typed data
			std::unique_ptr<IExtraData> TakeExtraObject()
			{
				if (GetType() == Type::Typed)
				{
					return std::move(std::get<static_cast<size_t>(Type::Typed)>(m_Data));
				}
				return nullptr;
			}
			IExtraData* GetExtraObject() const
			{
				if (GetType() == Type::Typed)
				{
					return std::get<static_cast<size_t>(Type::Typed)>(m_Data).get();
				}
				return nullptr;
			}
			void SetExtraObject(std::unique_ptr<IExtraData> extraObject)
			{
				m_Data = std::move(extraObject);
			}
	
		public:
			ExtraDataContainer& operator=(const ExtraDataContainer&) = delete;
			ExtraDataContainer& operator=(ExtraDataContainer&&) = default;
	};
}
