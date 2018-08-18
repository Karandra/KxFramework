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
