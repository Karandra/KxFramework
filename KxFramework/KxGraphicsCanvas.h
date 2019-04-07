#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxGraphicsCanvas
{
	public:
		enum class RendererType
		{
			None = -1,
			Direct2D,
			GDIPlus,
			Cairo,
		};

	private:
		wxDC& m_DC;
		wxGCDC& m_GDC;
		wxGraphicsContext* m_GraphicsContext = nullptr;
		RendererType m_RendererType = RendererType::None;

	protected:
		wxGraphicsRenderer& GetDefaultRenderer() const
		{
			return *wxGraphicsRenderer::GetDirect2DRenderer();
		}
		template<class T> wxGraphicsContext& CreateContext(wxGraphicsRenderer* renderer, T&& arg) const
		{
			return renderer ? *renderer->CreateContext(arg) : *GetDefaultRenderer().CreateContext(arg);
		}
		wxGraphicsContext& CreateContext(wxGraphicsRenderer* renderer, wxImage& image) const
		{
			return renderer ? *renderer->CreateContextFromImage(image) : *GetDefaultRenderer().CreateContextFromImage(image);
		}
		
		RendererType TestRendererType() const
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
					return RendererType::Direct2D;
				}
				else if (Test(wxGraphicsRenderer::GetGDIPlusRenderer()))
				{
					return RendererType::GDIPlus;
				}
				else if (Test(wxGraphicsRenderer::GetCairoRenderer()))
				{
					return RendererType::Cairo;
				}
			}
			return RendererType::None;
		}
		void InitCanvas()
		{
			m_GraphicsContext = m_GDC.GetGraphicsContext();
			m_RendererType = TestRendererType();
		}

	public:
		KxGraphicsCanvas(wxDC& dc, wxGCDC& gdc)
			:m_DC(dc), m_GDC(gdc)
		{
		}
		virtual ~KxGraphicsCanvas() = default;

	public:
		virtual wxBitmap GetBitmap()
		{
			Flush();
			return m_GDC.GetAsBitmap();
		}
		virtual wxSize GetSize() const
		{
			return m_GDC.GetSize();
		}
		wxRect GetRect() const
		{
			return wxRect({0, 0}, GetSize());
		}

		RendererType GetRendererType() const
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

		template<class TFunctor> void DrawIndirect(const wxRect& rect, TFunctor&& func)
		{
			wxBitmap bitmap(rect.GetSize(), 32);
			bitmap.UseAlpha(true);
			{
				wxMemoryDC memoryDC(bitmap);
				memoryDC.CopyAttributes(m_DC);

				wxRect rect2 = rect;
				rect2.SetPosition({0, 0});
				std::invoke(func, memoryDC, rect2);
			}
			m_GraphicsContext->DrawBitmap(bitmap, rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
		}
};

class KX_API KxPaintGraphicsCanvas: public KxGraphicsCanvas
{
	private:
		wxPaintDC m_PaintDC;
		wxGCDC m_GDC;

	public:
		KxPaintGraphicsCanvas(wxWindow* window, wxGraphicsRenderer* renderer = nullptr)
			:KxGraphicsCanvas(m_PaintDC, m_GDC), m_PaintDC(window), m_GDC(&CreateContext(renderer, m_PaintDC))
		{
			m_GDC.CopyAttributes(m_PaintDC);
			InitCanvas();
		}

	public:
		wxSize GetSize() const override
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
		wxRect GetUpdateClientRect() const
		{
			return GetWindow()->GetUpdateClientRect();
		}
};
