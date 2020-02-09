#pragma once
#include <wx/wx.h>
#include "SciterAPI.h"

namespace KxSciter
{
	struct NodeHandle;
	struct ElementHandle;
	struct ScriptNativeValue;

	struct GraphicsBitmapHandle;
}

namespace KxSciter
{
	std::vector<BYTE> ToSciterUTF8(const wxString& text);

	inline HELEMENT ToSciterElement(ElementHandle* handle)
	{
		return reinterpret_cast<HELEMENT>(handle);
	}
	inline ElementHandle* FromSciterElement(HELEMENT handle)
	{
		return reinterpret_cast<ElementHandle*>(handle);
	}

	inline HNODE ToSciterNode(NodeHandle* handle)
	{
		return reinterpret_cast<HNODE>(handle);
	}
	inline NodeHandle* FromSciterNode(HNODE handle)
	{
		return reinterpret_cast<NodeHandle*>(handle);
	}

	inline VALUE* ToSciterScriptValue(ScriptNativeValue& value)
	{
		return reinterpret_cast<VALUE*>(&value);
	}
	inline const VALUE* ToSciterScriptValue(const ScriptNativeValue& value)
	{
		return reinterpret_cast<const VALUE*>(&value);
	}
	inline ScriptNativeValue& FromSciterScriptValue(VALUE& value)
	{
		return *reinterpret_cast<ScriptNativeValue*>(&value);
	}

	inline HIMG ToSciterImage(GraphicsBitmapHandle* handle)
	{
		return reinterpret_cast<HIMG>(handle);
	}
	inline GraphicsBitmapHandle* FromSciterImage(HIMG handle)
	{
		return reinterpret_cast<GraphicsBitmapHandle*>(handle);
	}
}
