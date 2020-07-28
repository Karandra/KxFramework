#pragma once
#include "kxf/General/String.h"
#include "SciterAPI.h"

namespace kxf::Sciter
{
	struct NodeHandle;
	struct ElementHandle;
	struct ScriptNativeValue;

	struct GraphicsContextHandle;
	struct GraphicsBitmapHandle;
	struct GraphicsTextHandle;
	struct GraphicsPathHandle;
}

namespace kxf::Sciter
{
	std::vector<BYTE> ToSciterUTF8(const String& text);

	SC_COLOR CreateSciterColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept;
	SC_COLOR CreateSciterColor(const wxColour& color) noexcept;
	SC_COLOR CreateSciterColor(const Color& color) noexcept;
}

namespace kxf::Sciter
{
	inline HELEMENT ToSciterElement(ElementHandle* handle) noexcept
	{
		return reinterpret_cast<HELEMENT>(handle);
	}
	inline ElementHandle* FromSciterElement(HELEMENT handle) noexcept
	{
		return reinterpret_cast<ElementHandle*>(handle);
	}

	inline HNODE ToSciterNode(NodeHandle* handle) noexcept
	{
		return reinterpret_cast<HNODE>(handle);
	}
	inline NodeHandle* FromSciterNode(HNODE handle) noexcept
	{
		return reinterpret_cast<NodeHandle*>(handle);
	}

	inline VALUE* ToSciterScriptValue(ScriptNativeValue& value) noexcept
	{
		return reinterpret_cast<VALUE*>(&value);
	}
	inline const VALUE* ToSciterScriptValue(const ScriptNativeValue& value) noexcept
	{
		return reinterpret_cast<const VALUE*>(&value);
	}
	inline ScriptNativeValue& FromSciterScriptValue(VALUE& value) noexcept
	{
		return *reinterpret_cast<ScriptNativeValue*>(&value);
	}

	inline HGFX ToSciterGraphicsContext(GraphicsContextHandle* handle) noexcept
	{
		return reinterpret_cast<HGFX>(handle);
	}
	inline GraphicsContextHandle* FromSciterGraphicsContext(HGFX handle) noexcept
	{
		return reinterpret_cast<GraphicsContextHandle*>(handle);
	}
	
	inline HIMG ToSciterImage(GraphicsBitmapHandle* handle) noexcept
	{
		return reinterpret_cast<HIMG>(handle);
	}
	inline GraphicsBitmapHandle* FromSciterImage(HIMG handle) noexcept
	{
		return reinterpret_cast<GraphicsBitmapHandle*>(handle);
	}

	inline HTEXT ToSciterText(GraphicsTextHandle* handle) noexcept
	{
		return reinterpret_cast<HTEXT>(handle);
	}
	inline GraphicsTextHandle* FromSciterText(HTEXT handle) noexcept
	{
		return reinterpret_cast<GraphicsTextHandle*>(handle);
	}

	inline HPATH ToSciterPath(GraphicsPathHandle* handle) noexcept
	{
		return reinterpret_cast<HPATH>(handle);
	}
	inline GraphicsPathHandle* FromSciterPath(HPATH handle) noexcept
	{
		return reinterpret_cast<GraphicsPathHandle*>(handle);
	}
}
