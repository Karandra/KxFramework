#pragma once

#define	KxRTTI_ImplementClassDynamic(Name, CClassName, Base1CClassName)															\
wxClassInfo CClassName::ms_classInfo(wxT(#Name), &Base1CClassName::ms_classInfo, NULL, (int)sizeof(CClassName), NULL);			\
wxClassInfo* CClassName::GetClassInfo() const																					\
{																																\
	return &CClassName::ms_classInfo;																							\
}

#define	KxRTTI_ImplementClassDynamic2(Name, CClassName, Base1CClassName, Base2CClassName)															\
wxClassInfo CClassName::ms_classInfo(wxT(#Name), &Base1CClassName::ms_classInfo, &Base2CClassName::ms_classInfo, (int)sizeof(CClassName), NULL);	\
wxClassInfo* CClassName::GetClassInfo() const																										\
{																																					\
	return &CClassName::ms_classInfo;																												\
}

namespace KxRTTI
{
	template<class Base> class DynamicCastAsIs
	{
		public:
			virtual ~DynamicCastAsIs() = default;

		public:
			template<class T> bool As(T*& ptr)
			{
				static_assert(std::is_base_of<Base, T>::value, "T must be derived from 'KPluginEntry'");
				ptr = dynamic_cast<T*>(this);
				return ptr != NULL;
			}
			template<class T> bool As(const T*& ptr) const
			{
				static_assert(std::is_base_of<Base, T>::value, "T must be derived from 'KPluginEntry'");
				ptr = dynamic_cast<const T*>(this);
				return ptr != NULL;
			}

			template<class T> bool Is() const
			{
				const T*& ptr = NULL;
				return As<T>(ptr);
			}
	};
}
