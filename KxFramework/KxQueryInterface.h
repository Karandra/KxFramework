#pragma once
#include <typeinfo>

namespace KxRTTI
{
	using IID = std::size_t;

	template<class T> IID GetIIDOf() noexcept
	{
		return typeid(T).hash_code();
	}

	class IObject;
	class Query final
	{
		private:
			const IID m_IID;
			IObject* m_IObject = nullptr;
			void* m_ActualObject = nullptr;

		public:
			Query(const IID& iid)
				:m_IID(iid)
			{
			}

		public:
			const IID& GetIID() const
			{
				return m_IID;
			}
			bool TestIID(const IID& other) const
			{
				return m_IID == other;
			}
					
			IObject* GetIObject() const
			{
				return m_IObject;
			}
			void* GetActualObject() const
			{
				return m_ActualObject;
			}
			template<class T> T* GetActualObject() const
			{
				return reinterpret_cast<T*>(m_ActualObject);
			}
					
			void AssignNull()
			{
				m_IObject = nullptr;
				m_ActualObject = nullptr;
			}
			void Assign(IObject* object, void* actualObject)
			{
				m_IObject = object;
				m_ActualObject = actualObject;
			}
	};
}

namespace KxRTTI
{
	class IObject
	{
		protected:
			virtual bool OnQueryInterface(Query& query) noexcept = 0
			{
				static const IID ms_IID = GetIIDOf<IObject>();
				if (query.TestIID(ms_IID))
				{
					query.Assign(const_cast<IObject*>(this), const_cast<IObject*>(this));
					return true;
				}
				return false;
			}

		public:
			virtual ~IObject() = default;

		public:
			IObject* QueryInterface(const IID& iid) noexcept
			{
				Query query(iid);
				OnQueryInterface(query);
				return query.GetIObject();
			}
			const IObject* QueryInterface(const IID& iid) const noexcept
			{
				Query query(iid);
				const_cast<IObject*>(this)->OnQueryInterface(query);
				return query.GetIObject();
			}

			template<class T> T* QueryInterface() noexcept
			{
				Query query(GetIIDOf<T>());
				OnQueryInterface(query);
				return query.GetActualObject<T>();
			}
			template<class T> const T* QueryInterface() const noexcept
			{
				Query query(GetIIDOf<T>());
				const_cast<IObject*>(this)->OnQueryInterface(query);
				return query.GetActualObject<T>();
			}

			template<class T> bool QueryInterface(T*& ptr) noexcept
			{
				ptr = QueryInterface<T>();
				return ptr != nullptr;
			}
			template<class T> bool QueryInterface(const T*& ptr) const noexcept
			{
				ptr = QueryInterface<T>();
				return ptr != nullptr;
			}
	};
}

namespace KxRTTI
{
	template<class TInterface>
	class IInterface: public IObject
	{
		protected:
			bool OnQueryInterface(Query& query) noexcept override
			{
				static const IID ms_IID = GetIIDOf<TInterface>();
				if (query.TestIID(ms_IID))
				{
					query.Assign(const_cast<IInterface*>(this), const_cast<IInterface*>(this));
					return true;
				}
				return IObject::OnQueryInterface(query);
			}
	};

	template<class TInterface, class... TBaseInterface>
	class IExtendInterface: public IInterface<IExtendInterface<TInterface, TBaseInterface...>>, public TBaseInterface...
	{
		private:
			using IOW = IInterface<IExtendInterface<TInterface, TBaseInterface...>>;
			template<size_t N, typename... Ts> using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

		protected:
			bool OnQueryInterface(Query& query) noexcept override
			{
				static const IID ms_IID = GetIIDOf<TInterface>();
				if (query.TestIID(ms_IID))
				{
					query.Assign(GetIOW(), const_cast<IExtendInterface*>(this));
					return true;
				}
				return (TBaseInterface::OnQueryInterface(query) || ...);
			}

		private:
			IObject* GetIOW()
			{
				return static_cast<IOW*>(this);
			}
			const IObject* GetIOW() const
			{
				return static_cast<const IOW*>(this);
			}

		public:
			IExtendInterface() = default;
			template<class... Args> IExtendInterface(Args&&... arg)
				:NthTypeOf<0, TBaseInterface...>(std::forward<Args>(arg)...)
			{
			}

		public:
			IObject* QueryInterface(const IID& iid) noexcept
			{
				return GetIOW()->QueryInterface(iid);
			}
			const IObject* QueryInterface(const IID& iid) const noexcept
			{
				return GetIOW()->QueryInterface(iid);
			}

			template<class T> T* QueryInterface() noexcept
			{
				return GetIOW()->QueryInterface<T>();
			}
			template<class T> const T* QueryInterface() const noexcept
			{
				return GetIOW()->QueryInterface<T>();
			}

			template<class T> bool QueryInterface(T*& ptr) noexcept
			{
				return GetIOW()->QueryInterface(ptr);
			}
			template<class T> bool QueryInterface(const T*& ptr) const noexcept
			{
				return GetIOW()->QueryInterface(ptr);
			}

		public:
			operator IObject*()
			{
				return GetIOW();
			}
			operator const IObject*() const
			{
				return GetIOW();
			}
	};
}
