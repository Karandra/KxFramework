#include "KxfPCH.h"
#include "PaintEvent.h"
#include "kxf/Sciter/GraphicsRenderer/GraphicsContext.h"

namespace kxf::Sciter
{
	GraphicsContext PaintEvent::GetGraphicsContext() const
	{
		return m_GraphicsContext;
	}
	void PaintEvent::SetGraphicsContext(GraphicsContextHandle* handle)
	{
		m_GraphicsContext = handle;
	}
}
