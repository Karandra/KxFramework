/////////////////////////////////////////////////////////////////////////////
// Define user-provided macros
/////////////////////////////////////////////////////////////////////////////
#define wxNO_UNSAFE_WXSTRING_CONV 1

// Since v3.1.4 wxWidgets is using this macro to switch class access modifiers
// depending on whether it's built as a static or dynamic library. We need to
// always define it as otherwise it's causing linker errors due to name mangling.
#define WXBUILDING

/////////////////////////////////////////////////////////////////////////////
// Include common headers
/////////////////////////////////////////////////////////////////////////////
#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/log.h>
#include <wx/gdicmn.h>
#include <wx/dcgraph.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/renderer.h>
#include <wx/sysopt.h>
#include <wx/regex.h>
#include <wx/stream.h>
#include <wx/mstream.h>
#include <wx/datetime.h>
#include <wx/cmdline.h>

/////////////////////////////////////////////////////////////////////////////
// Options for wxWidgets that are required for KxFramework-based applications
//
// Refer to the original 'setup.h' file in the wxWidgets distribution for 
// options description.
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Compatibility settings
// ----------------------------------------------------------------------------

// Required because wxWidgets uses old CRT functions
static_assert(_CRT_SECURE_NO_DEPRECATE == 1);
static_assert(_CRT_SECURE_NO_WARNINGS == 1);
static_assert(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS == 1);

// None of the options below are strictly required
// static_assert(WXWIN_COMPATIBILITY_2_8 == 0);
// static_assert(WXWIN_COMPATIBILITY_3_0 == 0);
// static_assert(wxDIALOG_UNIT_COMPATIBILITY == 0);
// static_assert(wxUSE_UNSAFE_WXSTRING_CONV == 0);
// static_assert(wxNO_UNSAFE_WXSTRING_CONV == 1);

// ----------------------------------------------------------------------------
// Common options
// ----------------------------------------------------------------------------
static_assert(wxUSE_GUI == 1);
static_assert(wxUSE_UNICODE == 1);
static_assert(wxUSE_UNICODE_WCHAR == 1);

// ----------------------------------------------------------------------------
// Interoperability with the standard library
// ----------------------------------------------------------------------------
static_assert(wxUSE_STL == 1);
static_assert(wxUSE_STD_DEFAULT == 1);
static_assert(wxUSE_STD_CONTAINERS_COMPATIBLY == wxUSE_STD_DEFAULT);
static_assert(wxUSE_STD_CONTAINERS == wxUSE_STD_DEFAULT);
static_assert(wxUSE_STD_IOSTREAM == wxUSE_STD_DEFAULT);
static_assert(wxUSE_STD_STRING == wxUSE_STD_DEFAULT);
static_assert(wxUSE_STD_STRING_CONV_IN_WXSTRING == wxUSE_STL);

// ----------------------------------------------------------------------------
// Non GUI features selection
// ----------------------------------------------------------------------------
static_assert(wxUSE_INTL == 1);
static_assert(wxUSE_IPV6 == 1);

// ----------------------------------------------------------------------------
// Windows-only settings
// ----------------------------------------------------------------------------
static_assert(wxUSE_WINSOCK2 == 1);
