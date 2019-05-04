#pragma once
#include "KxFramework/KxFramework.h"
#include "IComponentContainer.h"
#include "ComponentItem.h"
#include <typeindex>

class KX_API KxComponentContainer: public KxIComponentContainer
{
	private:
		std::unordered_map<std::type_index, KxComponentItem> m_Components;

	private:
		void Clear();

	protected:
		KxComponent& DoAddComponent(KxComponentItem item) override;
		KxComponent* DoGetComponent(const std::type_info& typeInfo) override;
		KxComponent* DoRemoveComponent(const std::type_info& typeInfo) override;
		void DoRemoveAllComponents() override;
		void DoEnumComponents(TEnumFunction func) override;

	public:
		KxComponentContainer() = default;
		KxComponentContainer(KxComponentContainer&&) = default;
		KxComponentContainer(const KxComponentContainer&) = delete;
		virtual ~KxComponentContainer();

	public:
		KxComponentContainer& operator=(KxComponentContainer&&) = default;
		KxComponentContainer& operator=(const KxComponentContainer&) = delete;
};
