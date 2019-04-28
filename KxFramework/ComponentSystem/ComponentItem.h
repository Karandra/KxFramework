#pragma once
#include "KxFramework/KxFramework.h"
#include <typeinfo>
class KxComponent;
class KxIComponentContainer;

class KxComponentItem final
{
	friend class KxIComponentContainer;

	private:
		KxComponent* m_Component = nullptr;
		const std::type_info* m_TypeInfo = nullptr;
		bool m_NeedDelete = false;

	private:
		void Destroy();
		void MakeNull();

	private:
		KxComponentItem() = default;
		template<class TComponent> KxComponentItem(TComponent& component, const std::type_info* typeInfo = nullptr)
			:m_Component(&component), m_TypeInfo(typeInfo ? typeInfo : &typeid(component))
		{
		}
		template<class TComponent> KxComponentItem(std::unique_ptr<TComponent> component, const std::type_info* typeInfo = nullptr)
			:KxComponentItem(*component.release(), typeInfo)
		{
			m_NeedDelete = true;
		}
		
	public:
		KxComponentItem(const KxComponentItem&) = delete;
		KxComponentItem(KxComponentItem&& other) noexcept
		{
			*this = std::move(other);
		}
		~KxComponentItem()
		{
			Destroy();
		}

	public:
		KxComponent& Get()
		{
			return *m_Component;
		}
		const KxComponent& Get() const
		{
			return *m_Component;
		}

		const type_info& GetTypeInfo() const
		{
			return *m_TypeInfo;
		}
		template<class TComponent> bool IsOfType() const
		{
			return typeid(TComponent) == *m_TypeInfo;
		}
		template<class TComponent> bool IsOfType(TComponent*& ptr) const
		{
			if (IsOfType<TComponent>())
			{
				ptr = &AsType<TComponent>();
				return ptr != nullptr;
			}
			return false;
		}
		template<class TComponent> TComponent& AsType() const
		{
			return *static_cast<TComponent*>(m_Component);
		}

		bool IsNull() const
		{
			return m_Component == nullptr || m_TypeInfo == nullptr;
		}
		bool NeedDelete() const
		{
			return m_NeedDelete;
		}

	public:
		KxComponentItem& operator=(KxComponentItem&& other) noexcept
		{
			std::swap(m_Component, other.m_Component);
			std::swap(m_NeedDelete, other.m_NeedDelete);
			std::swap(m_TypeInfo, other.m_TypeInfo);
				
			other.MakeNull();
			return *this;
		}
};
