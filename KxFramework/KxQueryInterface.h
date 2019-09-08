#pragma once
#include <typeinfo>

class KX_API KxIObject;

namespace KxRTTI
{
	using IID = std::type_info;

	template<class T> const IID& IIDOf() noexcept
	{
		return typeid(T);
	}
}

namespace KxRTTI
{
	class KX_API QueryInfo final
	{
		private:
			const IID& m_IID;
			KxIObject* m_IObject = nullptr;
			void* m_ActualObject = nullptr;

		public:
			QueryInfo(const IID& iid) noexcept
				:m_IID(iid)
			{
			}

		public:
			const IID& GetIID() const noexcept
			{
				return m_IID;
			}
			bool TestIID(const IID& other) const noexcept
			{
				return m_IID == other;
			}
			
			KxIObject* GetIObject() const noexcept
			{
				return m_IObject;
			}
			void* GetActualObject() const noexcept
			{
				return m_ActualObject;
			}
			template<class T> T* GetActualObject() const noexcept
			{
				return reinterpret_cast<T*>(m_ActualObject);
			}
			
			void AssignNull() noexcept
			{
				m_IObject = nullptr;
				m_ActualObject = nullptr;
			}
			void Assign(KxIObject* object, void* actualObject) noexcept
			{
				m_IObject = object;
				m_ActualObject = actualObject;
			}
	};
}

class KX_API KxIObject
{
	public:
		using QueryInfo = KxRTTI::QueryInfo;
		using IID = KxRTTI::IID;

	public:
		template<class T> static const IID& IIDOf() noexcept
		{
			return KxRTTI::IIDOf<T>();
		}

	protected:
		virtual bool OnQueryInterface(QueryInfo& query) noexcept = 0
		{
			static const IID& s_IID = IIDOf<KxIObject>();
			if (query.TestIID(s_IID))
			{
				query.Assign(const_cast<KxIObject*>(this), const_cast<KxIObject*>(this));
				return true;
			}
			return false;
		}

	public:
		virtual ~KxIObject() = default;

	public:
		KxIObject* QueryInterface(const IID& iid) noexcept
		{
			QueryInfo query(iid);
			OnQueryInterface(query);
			return query.GetIObject();
		}
		const KxIObject* QueryInterface(const IID& iid) const noexcept
		{
			QueryInfo query(iid);
			const_cast<KxIObject*>(this)->OnQueryInterface(query);
			return query.GetIObject();
		}

		template<class T> T* QueryInterface() noexcept
		{
			QueryInfo query(IIDOf<T>());
			OnQueryInterface(query);
			return query.GetActualObject<T>();
		}
		template<class T> const T* QueryInterface() const noexcept
		{
			return const_cast<KxIObject&>(*this).QueryInterface<T>();
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

namespace KxRTTI
{
	template<class TInterface>
	class IInterface: public KxIObject
	{
		protected:
			bool OnQueryInterface(QueryInfo& query) noexcept override
			{
				static const IID& s_IID = IIDOf<TInterface>();
				if (query.TestIID(s_IID))
				{
					query.Assign(const_cast<IInterface*>(this), const_cast<IInterface*>(this));
					return true;
				}
				return KxIObject::OnQueryInterface(query);
			}
	};

	template<class TInterface, class... TBaseInterface>
	class IExtendInterface: public IInterface<IExtendInterface<TInterface, TBaseInterface...>>, public TBaseInterface...
	{
		private:
			using IOW = IInterface<IExtendInterface<TInterface, TBaseInterface...>>;
			template<size_t N, typename... Ts> using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

		protected:
			bool OnQueryInterface(QueryInfo& query) noexcept override
			{
				static const IID& s_IID = IIDOf<TInterface>();
				if (query.TestIID(s_IID))
				{
					query.Assign(GetIOW(), const_cast<IExtendInterface*>(this));
					return true;
				}
				return (TBaseInterface::OnQueryInterface(query) || ...);
			}

		private:
			KxIObject* GetIOW() noexcept
			{
				return static_cast<IOW*>(this);
			}
			const KxIObject* GetIOW() const noexcept
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
			KxIObject* QueryInterface(const IID& iid) noexcept
			{
				return GetIOW()->QueryInterface(iid);
			}
			const KxIObject* QueryInterface(const IID& iid) const noexcept
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
			operator KxIObject*() noexcept
			{
				return GetIOW();
			}
			operator const KxIObject*() const noexcept
			{
				return GetIOW();
			}
	};
}
