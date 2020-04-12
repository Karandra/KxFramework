/////////////////////////////////////////////////////////////////////////////
// Options for wxWidgets that are required for KxFramework-based applications
//
// Refer to the original 'setup.h' file in the wxWidgets distribution for 
// options description.
/////////////////////////////////////////////////////////////////////////////

// Suppress macro redefinition warning
#pragma warning(push, 0)   
#pragma warning(disable: 4005)

// ----------------------------------------------------------------------------
// Compatibility settings
// ----------------------------------------------------------------------------
#define WXWIN_COMPATIBILITY_2_8						0
#define WXWIN_COMPATIBILITY_3_0						0
#define wxDIALOG_UNIT_COMPATIBILITY					0
#define wxUSE_UNSAFE_WXSTRING_CONV					0
#define wxNO_UNSAFE_WXSTRING_CONV					1

// ----------------------------------------------------------------------------
// Interoperability with the standard library
// ----------------------------------------------------------------------------
#define wxUSE_STL									1
#define wxUSE_STD_DEFAULT							1
#define wxUSE_STD_CONTAINERS_COMPATIBLY				wxUSE_STD_DEFAULT
#define wxUSE_STD_CONTAINERS						wxUSE_STD_DEFAULT
#define wxUSE_STD_IOSTREAM							wxUSE_STD_DEFAULT
#define wxUSE_STD_STRING							wxUSE_STD_DEFAULT
#define wxUSE_STD_STRING_CONV_IN_WXSTRING			wxUSE_STL

// ----------------------------------------------------------------------------
// Non GUI features selection
// ----------------------------------------------------------------------------
#define wxUSE_INTL									0
#define wxUSE_IPV6									1

// ----------------------------------------------------------------------------
// Windows-only settings
// ----------------------------------------------------------------------------
#define wxUSE_WINSOCK2								1

// Restore any disabled warnings
#pragma warning(pop)
