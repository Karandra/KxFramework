#pragma once
#include <wx/wx.h>
#include "SciterAPI.h"

namespace KxSciter
{
	struct NodeHandle;
	struct ElementHandle;
	struct ScriptNativeValue;

	struct GraphicsContextHandle;
	struct GraphicsBitmapHandle;
	struct GraphicsTextHandle;
	struct GraphicsPathHandle;
}

namespace KxSciter
{
	std::vector<BYTE> ToSciterUTF8(const wxString& text);
	SC_COLOR CreateSciterColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	SC_COLOR CreateSciterColor(const wxColour& color);
}

namespace KxSciter
{
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

	inline HGFX ToSciterGraphicsContext(GraphicsContextHandle* handle)
	{
		return reinterpret_cast<HGFX>(handle);
	}
	inline GraphicsContextHandle* FromSciterGraphicsContext(HGFX handle)
	{
		return reinterpret_cast<GraphicsContextHandle*>(handle);
	}
	
	inline HIMG ToSciterImage(GraphicsBitmapHandle* handle)
	{
		return reinterpret_cast<HIMG>(handle);
	}
	inline GraphicsBitmapHandle* FromSciterImage(HIMG handle)
	{
		return reinterpret_cast<GraphicsBitmapHandle*>(handle);
	}

	inline HTEXT ToSciterText(GraphicsTextHandle* handle)
	{
		return reinterpret_cast<HTEXT>(handle);
	}
	inline GraphicsTextHandle* FromSciterText(HTEXT handle)
	{
		return reinterpret_cast<GraphicsTextHandle*>(handle);
	}

	inline HPATH ToSciterPath(GraphicsPathHandle* handle)
	{
		return reinterpret_cast<HPATH>(handle);
	}
	inline GraphicsPathHandle* FromSciterPath(HPATH handle)
	{
		return reinterpret_cast<GraphicsPathHandle*>(handle);
	}
}
