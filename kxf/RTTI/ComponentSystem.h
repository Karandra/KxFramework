#pragma once
#include "Common.h"
#include "QueryInterface.h"
#include <unordered_map>

namespace kxf::RTTI
{
	class IComponent;
	class IComponentContainer;

	class ComponentItem final
	{
		private:
			IComponent* m_Component = nullptr;
			bool m_NeedDelete = false;

		private:
			void Destroy() noexcept;
			void MakeNull() noexcept
			{
				m_Component = nullptr;
				m_NeedDelete = false;
			}

		public:
			ComponentItem() noexcept = default;
			ComponentItem(IComponent& component) noexcept
				:m_Component(&component)
			{
			}
			ComponentItem(std::unique_ptr<IComponent> component) noexcept
				:m_Component(component.release()), m_NeedDelete(true)
			{
			}
			ComponentItem(const ComponentItem&) = delete;
			ComponentItem(ComponentItem&& other) noexcept
			{
				*this = std::move(other);
			}
			~ComponentItem()
			{
				Destroy();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Component == nullptr;
			}
			bool NeedDelete() const noexcept
			{
				return m_NeedDelete;
			}

			IComponent* get() noexcept
			{
				return m_Component;
			}
			const IComponent* get() const noexcept
			{
				return m_Component;
			}

			IComponent* operator->() noexcept
			{
				return m_Component;
			}
			const IComponent* operator->() const noexcept
			{
				return m_Component;
			}

			IComponent& operator*() noexcept
			{
				return *m_Component;
			}
			const IComponent& operator*() const noexcept
			{
				return *m_Component;
			}

		public:
			ComponentItem& operator=(ComponentItem&& other) noexcept
			{
				std::swap(m_Component, other.m_Component);
				std::swap(m_NeedDelete, other.m_NeedDelete);
				
				other.MakeNull();
				return *this;
			}
	};
}

namespace kxf::RTTI
{
	class KX_API IComponent: public Interface<IComponent>
	{
		friend class ComponentItem;
		friend class ComponentContainer;
		KxDeclareIID(IComponent, {0x8ac99b20, 0xc73f, 0x4707, {0x98, 0x9e, 0xbb, 0x11, 0xfd, 0x48, 0x3f, 0x9d}});

		protected:
			virtual void OnInit()
			{
			}
			virtual void OnUninit()
			{
			}
			virtual void OnDestroy() noexcept
			{
				delete this;
			}
			virtual void OnQuery(const IID& iid) noexcept
			{
			}

			virtual IComponentContainer* OnGetContainer() const noexcept = 0;
			virtual void OnContainerChanged(IComponentContainer* container) noexcept = 0;

		public:
			~IComponent() = default;

		public:
			template<class TContainer = IComponentContainer>
			TContainer* GetContainer() const noexcept
			{
				return static_cast<TContainer*>(OnGetContainer());
			}
	};

	class KX_API IComponentContainer: public Interface<IComponentContainer>
	{
		KxDeclareIID(IComponentContainer, {0xd82d81d0, 0xf63e, 0x481d, {0x92, 0xee, 0x16, 0xd5, 0x34, 0xf1, 0x58, 0x77}});

		protected:
			virtual IComponent* DoQueryComponent(const IID& iid) noexcept = 0;
			virtual IComponent* DoAddComponent(ComponentItem item) = 0;
			virtual bool DoRemoveComponent(const IID& iid) = 0;

		public:
			~IComponentContainer() = default;

		public:
			// Query
			IComponent* QueryComponent(const IID& iid) noexcept
			{
				return DoQueryComponent(iid);
			}
			const IComponent* QueryComponent(const IID& iid) const noexcept
			{
				return const_cast<IComponentContainer&>(*this).QueryComponent(iid);
			}

			template<class T>
			T* QueryComponent() noexcept
			{
				return static_cast<T*>(this->QueryComponent(IID::FromType<T>()));
			}

			template<class T>
			const T* QueryComponent() const noexcept
			{
				return static_cast<const T*>(this->QueryComponent(IID::FromType<T>()));
			}

			template<class T>
			bool QueryComponent(T*& ptr) noexcept
			{
				ptr = this->QueryComponent<T>();
				return ptr != nullptr;
			}

			template<class T>
			bool QueryComponent(const T*& ptr) const noexcept
			{
				ptr = this->QueryComponent<T>();
				return ptr != nullptr;
			}

			// Add
			void AddComponent(IComponent& component)
			{
				DoAddComponent(component);
			}
			void AddComponent(std::unique_ptr<IComponent> component)
			{
				DoAddComponent(std::move(component));
			}
			
			// Remove
			bool RemoveComponent(const IID& iid)
			{
				return DoRemoveComponent(iid);
			}

			template<class T>
			bool RemoveComponent()
			{
				return DoRemoveComponent(IID::FromType<T>());
			}
	};
}

namespace kxf::RTTI
{
	class KX_API Component: public IComponent
	{
		private:
			IComponentContainer* m_Container = nullptr;

		protected:
			IComponentContainer* OnGetContainer() const noexcept override
			{
				return m_Container;
			}
			void OnContainerChanged(IComponentContainer* container) noexcept override
			{
				m_Container = container;
			}

		public:
			Component() = default;
			Component(const Component&) = delete;

		public:
			Component& operator=(const Component&) = delete;
	};

	template<class TContainer>
	class KX_API ComponentOf: public Component
	{
		public:
			ComponentOf() = default;

		public:
			TContainer* GetContainer() const noexcept
			{
				return static_cast<TContainer*>(Component::OnGetContainer());
			}
	};

	class KX_API ComponentContainer: public IComponentContainer
	{
		private:
			std::unordered_map<IID, ComponentItem> m_Components;

		protected:
			IComponent* DoQueryComponent(const IID& iid) noexcept override
			{
				if (auto it = m_Components.find(iid); it != m_Components.end())
				{
					ComponentItem& item = it->second;
					item->OnQuery(iid);

					return item.get();
				}
				return nullptr;
			}
			IComponent* DoAddComponent(ComponentItem item) override
			{
				if (!item.IsNull() && !item->GetContainer())
				{
					auto [it, added] = m_Components.emplace(item->GetIID(), std::move(item));
					if (added)
					{
						ComponentItem& item = it->second;
						item->OnContainerChanged(this);
						item->OnInit();

						return item.get();
					}
				}
				return nullptr;
			}
			bool DoRemoveComponent(const IID& iid) override
			{
				if (auto it = m_Components.find(iid); it != m_Components.end())
				{
					ComponentItem& item = it->second;
					item->OnUninit();
					item->OnContainerChanged(nullptr);
					if (item.NeedDelete())
					{
						item->OnDestroy();
						item = {};
					}

					m_Components.erase(it);
					return true;
				}
				return false;
			}

		public:
			ComponentContainer() = default;
			ComponentContainer(ComponentContainer&&) = default;
			ComponentContainer(const ComponentContainer&) = delete;
			~ComponentContainer()
			{
				for (auto&[iid, item]: m_Components)
				{
					item->OnUninit();
					if (item.NeedDelete())
					{
						item->OnDestroy();
						item = {};
					}
				}
			}

		public:
			ComponentContainer& operator=(ComponentContainer&&) = default;
			ComponentContainer& operator=(const ComponentContainer&) = delete;
	};
}
