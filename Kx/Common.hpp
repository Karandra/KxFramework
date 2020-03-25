#pragma once

#if defined KXFRAMEWORK_LIBRARY
#define KX_API __declspec(dllexport)
#elif defined KXFRAMEWORK_PLUGIN
#define KX_API __declspec(dllimport)
#else
#define KX_API
#endif

// Standard library
#include <cstdint>
#include <utility>
#include <memory>

// Some generic headers
#include <KxFramework/KxEnumClassOperations.h>

namespace KxFramework
{
	// Yes, this is the intended way of using 'KxEnumClassOperations' when you have your own namespace
	using namespace KxEnumClassOperations;
}
