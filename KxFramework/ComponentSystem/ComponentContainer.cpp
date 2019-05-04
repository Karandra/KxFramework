#include "KxStdAfx.h"
#include "ComponentContainer.h"
#include "Component.h"

void KxComponentContainer::Clear()
{
	// Call uninit for all components, they are going to be destroyed (or detached) on 'm_Components' clear
	for (auto& [key, value]: m_Components)
	{
		KxComponent& component = value.Get();
		component.OnUninit();
		component.m_Container = nullptr;
	}
	m_Components.clear();
}

KxComponent& KxComponentContainer::DoAddComponent(KxComponentItem item)
{
	// Add to container
	const std::type_index index = item.GetTypeInfo();
	auto it = m_Components.insert_or_assign(index, std::move(item));

	// Link the component with container
	KxComponent& component = (it.first->second).Get();
	component.m_Container = this;

	// Call init and return
	component.OnInit();
	return component;
}
KxComponent* KxComponentContainer::DoGetComponent(const std::type_info& typeInfo)
{
	auto it = m_Components.find(typeInfo);
	if (it != m_Components.end())
	{
		return &(it->second).Get();
	}
	return nullptr;
}
KxComponent* KxComponentContainer::DoRemoveComponent(const std::type_info& typeInfo)
{
	auto it = m_Components.find(typeInfo);
	if (it != m_Components.end())
	{
		KxComponentItem& item = it->second;
		KxComponent* component = &item.Get();
		const bool deleteNeeded = item.NeedDelete();

		// Call event function and unlink the component
		component->OnUninit();
		component->m_Container = nullptr;

		// Erase the item (and delete the component if owned)
		m_Components.erase(it);

		// If we don't own this component (and is wasn't deleted by call to 'erase'), return pointer to it
		if (!deleteNeeded)
		{
			return component;
		}
	}
	return nullptr;
}
void KxComponentContainer::DoRemoveAllComponents()
{
	Clear();
}
void KxComponentContainer::DoEnumComponents(TEnumFunction func)
{
	for (auto& [key, value]: m_Components)
	{
		if (!func(value))
		{
			return;
		}
	}
}

KxComponentContainer::~KxComponentContainer()
{
	Clear();
}
