#include "KxStdAfx.h"
#include "PaintEvent.h"
#include "Kx/Sciter/GraphicsRenderer/GraphicsContext.h"

namespace KxSciter
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
