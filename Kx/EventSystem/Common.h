#pragma once
#include "Kx/Common.hpp"
#include <wx/event.h>
#include <cstdint>
#include <memory>

using KxEventID = wxEventType;
template<class T> using KxEventTag = wxEventTypeTag<T>;
