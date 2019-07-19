#pragma once
#include "Kx/Common.hpp"
#include <cstdint>
#include <memory>
#include <wx/event.h>

using KxEventID = wxEventType;
template<class T> using KxEventTag = wxEventTypeTag<T>;
