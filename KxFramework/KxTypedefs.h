#pragma once
#include "KxIncludes.h"

// Typedefs
using wxStringView = std::basic_string_view<wxChar>;

using KxStringVector = std::vector<wxString>;
using KxStdStringVector = std::vector<std::string>;

using KxIntVector = std::vector<int>;
using KxIntPtrVector = std::vector<intptr_t>;
using KxSizeTVector = std::vector<size_t>;

using KxInt8Vector = std::vector<int8_t>;
using KxInt16Vector = std::vector<int16_t>;
using KxInt32Vector = std::vector<int32_t>;
using KxInt64Vector = std::vector<int64_t>;

using KxUInt8Vector = std::vector<uint8_t>;
using KxUInt16Vector = std::vector<uint16_t>;
using KxUInt32Vector = std::vector<uint32_t>;
using KxUInt64Vector = std::vector<uint64_t>;

using KxAnyVector = std::vector<wxAny>;

using KxStringToStringMap = std::map<wxString, wxString>;
using KxStringToStringUMap = std::unordered_map<wxString, wxString>;
