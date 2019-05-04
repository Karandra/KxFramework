#pragma once
#include "KxFramework/KxFramework.h"
#include "ComponentItem.h"
class KX_API KxComponent;

class KX_API KxIComponentContainer
{
	friend class KxComponent;

	protected:
		using TEnumFunction = std::function<bool(KxComponentItem&)>;

	protected:
		virtual KxComponent& DoAddComponent(KxComponentItem item) = 0;
		virtual KxComponent* DoGetComponent(const std::type_info& typeInfo) = 0;
		virtual KxComponent* DoRemoveComponent(const std::type_info& typeInfo) = 0;
		virtual void DoRemoveAllComponents() = 0;
		virtual void DoEnumComponents(TEnumFunction func) = 0;

	public:
		template<class TComponent> TComponent& AddComponent(TComponent& component)
		{
			DoAddComponent(KxComponentItem(component, &typeid(TComponent)));
			return component;
		}
		template<class TComponent> TComponent& AddComponent(std::unique_ptr<TComponent> component)
		{
			TComponent& ref = *component;
			DoAddComponent(KxComponentItem(std::move(component), &typeid(TComponent)));
			return ref;
		}
		template<class TComponent, class... Args> TComponent& NewComponent(Args&& ... arg)
		{
			return AddComponent<TComponent>(std::make_unique<TComponent>(std::forward<Args>(arg)...));
		}

		template<class TComponent> TComponent* RemoveComponent()
		{
			return static_cast<TComponent*>(DoRemoveComponent(typeid(TComponent)));
		}
		void RemoveAllComponents()
		{
			DoRemoveAllComponents();
		}

	public:
		template<class TComponent> TComponent* TryGetComponent()
		{
			return static_cast<TComponent*>(DoGetComponent(typeid(TComponent)));
		}
		template<class TComponent> const TComponent* TryGetComponent() const
		{
			return const_cast<KxIComponentContainer*>(this)->TryGetComponent<TComponent>();
		}

		template<class TComponent> bool TryGetComponent(TComponent*& ptr)
		{
			ptr = TryGetComponent<TComponent>();
			return ptr != nullptr;
		}
		template<class TComponent> bool TryGetComponent(const TComponent*& ptr) const
		{
			ptr = TryGetComponent<TComponent>();
			return ptr != nullptr;
		}

		template<class TComponent> TComponent& GetComponent()
		{
			return *TryGetComponent<TComponent>();
		}
		template<class TComponent> const TComponent& GetComponent() const
		{
			return *TryGetComponent<TComponent>();
		}

		template<class TComponent> bool HasComponent() const
		{
			return TryGetComponent<TComponent>() != nullptr;
		}

	public:
		template<class TFunctor> void EnumComponents(TFunctor&& func)
		{
			EnumComponentItems([func = std::move(func)](KxComponentItem& item)
			{
				return std::invoke(func, item.Get());
			});
		}
		template<class TFunctor> void EnumComponentItems(TFunctor&& func)
		{
			if constexpr(std::is_same_v<std::invoke_result_t<TFunctor, KxComponentItem&>, bool>)
			{
				DoEnumComponents(std::move(func));
			}
			else
			{
				DoEnumComponents([func = std::move(func)](KxComponentItem& item)
				{
					std::invoke(func, item);
					return true;
				});
			}
		}
};
