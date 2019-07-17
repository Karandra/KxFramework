#pragma once

// Misc
#define KX_COMCALL STDMETHODCALLTYPE

#if defined KXFRAMEWORK_LIBRARY
#define KX_API __declspec(dllexport)
#elif defined KXFRAMEWORK_PLUGIN
#define KX_API __declspec(dllimport)
#else
#define KX_API
#endif
