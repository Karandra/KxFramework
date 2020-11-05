#pragma once
#include "Common.h"
#include "Geometry.h"
#include "Bitmap.h"
#include "Image.h"
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dcgraph.h>
#include <wx/graphics.h>
#include <wx/window.h>

namespace kxf
{
	class KX_API GraphicsCanvas
	{
		private:
			wxDC& m_DC;
			wxGCDC& m_GDC;
			wxGraphicsContext* m_GraphicsContext = nullptr;
			GraphicsRendererType m_RendererType = GraphicsRendererType::None;

		protected:
			wxGraphicsRenderer& GetDefaultRenderer() const
			{
				return *wxGraphicsRenderer::GetDirect2DRenderer();
			}
			
			template<class T>
			wxGraphicsContext& CreateContext(wxGraphicsRenderer* renderer, T&& arg) const
			{
				return renderer ? *renderer->CreateContext(arg) : *GetDefaultRenderer().CreateContext(arg);
			}
			
			wxGraphicsContext& CreateContext(wxGraphicsRenderer* renderer, Image& image) const
			{
				return renderer ? *renderer->CreateContextFromImage(image.ToWxImage()) : *GetDefaultRenderer().CreateContextFromImage(image.ToWxImage());
			}
		
			GraphicsRendererType TestRendererType() const
			{
				wxGraphicsRenderer* thisRenderer = m_GraphicsContext->GetRenderer();
				if (thisRenderer)
				{
					auto Test = [thisRenderer](wxGraphicsRenderer* renderer)
					{
						return renderer != nullptr && thisRenderer == renderer;
					};

					if (Test(wxGraphicsRenderer::GetDirect2DRenderer()))
					{
						return GraphicsRendererType::Direct2D;
					}
					else if (Test(wxGraphicsRenderer::GetGDIPlusRenderer()))
					{
						return GraphicsRendererType::GDIPlus;
					}
					else if (Test(wxGraphicsRenderer::GetCairoRenderer()))
					{
						return GraphicsRendererType::Cairo;
					}
				}
				return GraphicsRendererType::None;
			}
			void InitCanvas()
			{
				m_GraphicsContext = m_GDC.GetGraphicsContext();
				m_RendererType = TestRendererType();
			}

		public:
			GraphicsCanvas(wxDC& dc, wxGCDC& gdc)
				:m_DC(dc), m_GDC(gdc)
			{
			}
			virtual ~GraphicsCanvas() = default;

		public:
			virtual Bitmap GetBitmap()
			{
				Flush();
				return m_GDC.GetAsBitmap();
			}
			virtual Size GetSize() const
			{
				return m_GDC.GetSize();
			}
			Rect GetRect() const
			{
				return Rect({0, 0}, GetSize());
			}

			GraphicsRendererType GetRendererType() const
			{
				return m_RendererType;
			}
			wxGraphicsRenderer& GetRenderer() const
			{
				return *m_GraphicsContext->GetRenderer();
			}
			wxGraphicsContext& GetContext() const
			{
				return *m_GraphicsContext;
			}
		
			wxDC& GetDC() const
			{
				return m_DC;
			}
			wxGCDC& GetGDC() const
			{
				return m_GDC;
			}

		public:
			void Clear()
			{
				m_GDC.Clear();
			}
			void Flush()
			{
				m_GraphicsContext->Flush();
			}

			template<class TFunc>
			void DrawIndirect(const Rect& rect, TFunc&& func)
			{
				Bitmap bitmap(rect.GetSize(), 32);
				bitmap.UseAlpha(true);
				{
					wxMemoryDC memoryDC(bitmap);
					memoryDC.CopyAttributes(m_DC);

					Rect rect2 = rect;
					rect2.SetPosition({0, 0});
					std::invoke(func, memoryDC, std::move(rect2));
				}
				m_GraphicsContext->DrawBitmap(bitmap, rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			}
	};
}

namespace kxf
{
	class KX_API PaintGraphicsCanvas: public GraphicsCanvas
	{
		private:
			wxPaintDC m_PaintDC;
			wxGCDC m_GDC;

		public:
			PaintGraphicsCanvas(wxWindow* window, wxGraphicsRenderer* renderer = nullptr)
				:GraphicsCanvas(m_PaintDC, m_GDC), m_PaintDC(window), m_GDC(&CreateContext(renderer, m_PaintDC))
			{
				m_GDC.CopyAttributes(m_PaintDC);
				InitCanvas();
			}

		public:
			Size GetSize() const override
			{
				return GetWindow()->GetClientSize();
			}
			wxWindow* GetWindow() const
			{
				return m_PaintDC.GetWindow();
			}
			wxRegion& GetUpdateRegion() const
			{
				return GetWindow()->GetUpdateRegion();
			}
			Rect GetUpdateClientRect() const
			{
				return GetWindow()->GetUpdateClientRect();
			}
	};
}
