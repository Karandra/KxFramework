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

		private:
			template<class T>
			static constexpr void AssertUntypedStorageType()
			{
				using Tx = std::remove_reference_t<T>;
				static_assert(sizeof(Tx) <= sizeof(void*) && (std::is_null_pointer_v<Tx> || std::is_pointer_v<Tx> || std::is_integral_v<Tx> || std::is_enum_v<Tx> || std::is_floating_point_v<Tx>), "invalid type for untyped storage");
			}

		public:
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
				AssertUntypedStorageType<T>();

				if (GetType() == Type::Untyped)
				{
					void* data = std::get<static_cast<size_t>(Type::Untyped)>(m_Data);
					return reinterpret_cast<T>(data);
				}
				return T{};
			}

			template<class T>
			void SetExtraData(T&& data)
			{
				AssertUntypedStorageType<T>();

				m_Data = reinterpret_cast<void*>(data);
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
	};
}
