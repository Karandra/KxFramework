#pragma once
#include "Common.h"
#include "IObject.h"
#include "ClassInfo.h"
#include "kxf/Utility/TypeTraits.h"

namespace kxf::RTTI
{
	template<class T>
	class Interface: public virtual IObject
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		private:
			static inline RTTI::ClassInfoOf<T, IObject> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					return static_cast<ClassInfo*>(&ms_ClassInfo);
				}
				return IObject::QuerySelf(iid, static_cast<T&>(*this));
			}

		public:
			Interface() = default;
	};

	template<class TDerived, class... TBase>
	class ExtendInterface: public TBase...
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		protected:
			using TBaseInterface = typename ExtendInterface<TDerived, TBase...>;

		private:
			static inline RTTI::ClassInfoOf<TDerived, TBase...> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");

				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					return static_cast<ClassInfo*>(&ms_ClassInfo);
				}
				return IObject::QuerySelf<TBase...>(iid, static_cast<TDerived&>(*this));
			}

		public:
			ExtendInterface() = default;
	};

	template<class TDerived, class... TBase>
	class ImplementInterface: public TBase...
	{
		template<class T>
		friend const ClassInfo& GetClassInfo() noexcept;

		protected:
			using TBaseClass = typename ImplementInterface<TDerived, TBase...>;

		private:
			static inline RTTI::ClassInfoOfImplementation<TDerived, TBase...> ms_ClassInfo;

		protected:
			RTTI::QueryInfo DoQueryInterface(const IID& iid) noexcept override
			{
				static_assert((std::is_base_of_v<IObject, TBase> && ...), "[...] must inherit from 'IObject'");

				if (iid.IsOfType<RTTI::ClassInfo>())
				{
					return static_cast<ClassInfo*>(&ms_ClassInfo);
				}
				else if (RTTI::QueryInfo ptr; ((ptr = TBase::DoQueryInterface(iid), !ptr.is_null()) || ...))
				{
					return ptr;
				}
				return nullptr;
			}

		public:
			ImplementInterface() = default;
	};
}

#define KxRTTI_QueryInterface_Extend(T, ...)	\
\
private:	\
	static inline kxf::RTTI::ClassInfoOf<T, __VA_ARGS__> ms_ClassInfo; \
	\
public:	\
	kxf::RTTI::QueryInfo DoQueryInterface(const kxf::IID& iid) noexcept override	\
	{	\
		if (iid.IsOfType<kxf::RTTI::ClassInfo>())	\
		{	\
			return static_cast<kxf::RTTI::ClassInfo*>(&ms_ClassInfo);	\
		}	\
		return kxf::IObject::QuerySelf<__VA_ARGS__>(iid, static_cast<T&>(*this));	\
	}

#define KxRTTI_QueryInterface_Base(T)	KxRTTI_QueryInterface_Extend(T)
