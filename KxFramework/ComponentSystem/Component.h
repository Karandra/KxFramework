#pragma once
#include "KxFramework/KxFramework.h"
#include "IComponentContainer.h"
#include "ComponentItem.h"

class KX_API KxComponent: public KxIComponentContainer
{
	friend class KxComponentContainer;
	friend class KxComponentItem;

	private:
		KxIComponentContainer* m_Container = nullptr;

	protected:
		KxComponent& DoAddComponent(KxComponentItem item) override
		{
			return m_Container->DoAddComponent(std::move(item));
		}
		KxComponent* DoGetComponent(const std::type_info& typeInfo) override
		{
			return m_Container->DoGetComponent(typeInfo);
		}
		KxComponent* DoRemoveComponent(const std::type_info& typeInfo) override
		{
			return m_Container->DoRemoveComponent(typeInfo);
		}
		void DoRemoveAllComponents() override
		{
			m_Container->DoRemoveAllComponents();
		}
		void DoEnumComponents(TEnumFunction func) override
		{
			m_Container->DoEnumComponents(std::move(func));
		}

	protected:
		virtual void Delete()
		{
			delete this;
		}
		virtual void OnInit()
		{
		}
		virtual void OnUninit()
		{
		}

	public:
		virtual ~KxComponent() = default;

	public:
		bool IsLinkedToContainer() const
		{
			return m_Container != nullptr;
		}
		
		template<class TContainer = KxIComponentContainer> TContainer& GetContainer()
		{
			static_assert(std::is_base_of_v<KxIComponentContainer, TContainer>);
			return *static_cast<TContainer*>(m_Container);
		}
		template<class TContainer = KxIComponentContainer> const TContainer& GetContainer() const
		{
			static_assert(std::is_base_of_v<KxIComponentContainer, TContainer>);
			return *static_cast<const TContainer*>(m_Container);
		}
};

template<class TBaseContainer> class KxComponentOf: public KxComponent
{
	public:
		template<class TContainer = TBaseContainer> TContainer& GetContainer()
		{
			return KxComponent::GetContainer<TContainer>();
		}
		template<class TContainer = TBaseContainer> const TContainer& GetContainer() const
		{
			return KxComponent::GetContainer<TContainer>();
		}
};
