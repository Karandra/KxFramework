#pragma once
#include "Common.h"
#include "ExtraDataPrivate.h"
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
				return Private::ExtraData::GetUntypedData<T>(m_Data);
			}

			template<class T>
			void SetExtraData(T&& data)
			{
				m_Data = Private::ExtraData::SetUntypedData(std::forward<T>(data));
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
					return Private::ExtraData::GetUntypedData<T>(data);
				}
				return T{};
			}

			template<class T>
			void SetExtraData(T&& data)
			{
				m_Data = Private::ExtraData::SetUntypedData(std::forward<T>(data));
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
