#pragma once
#include "Kx/Sciter/Common.h"
#include "Kx/Sciter/Utility/HandleWrapper.h"

namespace kxf::Sciter
{
	struct GraphicsContextHandle;
	class ScriptValue;
	class GraphicsText;
	class GraphicsPath;
	class GraphicsBitmap;
	class GraphicsMatrix;
}

namespace kxf::Sciter
{
	class KX_API GraphicsMatrix final
	{
		friend class GraphicsContext;

		private:
			double m_11 = 1.0;
			double m_12 = 0.0;
			double m_21 = 0.0;
			double m_22 = 1.0;
			double m_dx = 0.0;
			double m_dy = 0.0;

		public:
			GraphicsMatrix() = default;
			GraphicsMatrix(double a, double b, double c, double d, double tx, double ty)
				:m_11(a), m_12(b), m_21(c), m_22(d), m_dx(tx), m_dy(ty)
			{
			}

		public:
			void Get(double& a, double& b, double& c, double& d, double& tx, double& ty)
			{
				a = m_11;
				b = m_12;
				c = m_21;
				d = m_22;
				tx = m_dx;
				ty = m_dy;
			}
	};
}

namespace kxf::Sciter
{
	class KX_API GraphicsContext final: public HandleWrapper<GraphicsContext, GraphicsContextHandle>
	{
		friend class HandleWrapper<GraphicsContext, GraphicsContextHandle>;
		
		private:
			GraphicsContextHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(GraphicsContextHandle* handle);
			void DoRelease();

		public:
			GraphicsContext() = default;
			GraphicsContext(GraphicsContextHandle* handle)
				:HandleWrapper(handle)
			{
			}
			GraphicsContext(const GraphicsContext& other)
				:HandleWrapper(other)
			{
			}
			GraphicsContext(GraphicsContext&& other)
				:HandleWrapper(std::move(other))
			{
			}
			GraphicsContext(GraphicsBitmap& bitmap);
			GraphicsContext(const ScriptValue& value);

		public:
			ScriptValue ToScriptValue() const;

			// Drawing functions
			GraphicsPath CreatePath();
			void DrawPath(const GraphicsPath& path, wxPolygonFillMode fillStyle = wxODDEVEN_RULE);
			void FillPath(const GraphicsPath& path, wxPolygonFillMode fillStyle = wxODDEVEN_RULE);
			void StrokePath(const GraphicsPath& path);

			void DrawEllipse(const wxRect2DDouble& rect);
			void DrawRectangle(const wxRect2DDouble& rect);
			void DrawRoundedRectangle(const wxRect2DDouble& rect, double radius);
			void DrawArc(const wxPoint2DDouble& p, const wxPoint2DDouble& r, double start, double sweep);
			void DrawStar(const wxPoint2DDouble& p, const wxPoint2DDouble& r, double start, int raysCount);
			
			void StrokeLine(const wxPoint2DDouble& p1, const wxPoint2DDouble& p2);
			void StrokeLines(const wxPoint2DDouble* points, size_t count);
			void StrokeLines(const wxPoint2DDouble* beginPoints, const wxPoint2DDouble* endPoints, size_t count);

			wxRect2DDouble DrawBitmap(const GraphicsBitmap& bitmap, const wxRect2DDouble& rect, double opacity = 1.0);
			wxRect2DDouble DrawBitmap(const GraphicsBitmap& bitmap, const wxPoint2DDouble& pos, double opacity = 1.0)
			{
				return DrawBitmap(bitmap, wxRect2DDouble(pos.m_x, pos.m_y, -1, -1), opacity);
			}

			void DrawText(const GraphicsText& text, const wxPoint2DDouble& pos, FlagSet<Alignment> alignment = Alignment::None)
			{
				DrawText(text, pos, MapCornerAlignment(alignment));
			}
			void DrawText(const GraphicsText& text, const wxPoint2DDouble& pos, CornerAlignment alignment = CornerAlignment::Unspecified);

			// Brush and pen functions
			void SetPen(const wxPen& pen);
			void SetPen(const wxGraphicsPenInfo& pen);
			void ResetPen();

			void SetBrush(const wxBrush& brush);
			void SetBrush(const wxGraphicsPenInfo& brush);
			void ResetBrush();

			// Transformation matrix
			wxPoint2DDouble Rotate(double angle);
			void Scale(const wxPoint2DDouble& scale);
			void Scale(double xScale, double yScale)
			{
				Scale(wxPoint2DDouble(xScale, yScale));
			}
			void Translate(const wxPoint2DDouble& p);
			void Translate(double dx, double dy)
			{
				Translate(wxPoint2DDouble(dx, dy));
			}

			GraphicsMatrix CreateMatrix(double a = 1.0, double b = 0.0, double c = 0.0, double d = 1.0, double tx = 0.0, double ty = 0.0);
			void Transform(const GraphicsMatrix& matrix);

			// Coordinate space
			wxPoint2DDouble WorldToScreen(const wxPoint2DDouble& p) const;
			wxPoint2DDouble ScreenToWorld(const wxPoint2DDouble& p) const;

			// Clipping region functions
			void PushClip(const wxRect2DDouble& rect, double opacity = 1.0);
			void PushClip(const GraphicsPath& path, double opacity = 1.0);
			void PopClip();

			// Modifying the state
			void Flush();
			void PushState();
			void PopState();

		public:
			GraphicsContext& operator=(const GraphicsContext& other)
			{
				CopyFrom(other);
				return *this;
			}
			GraphicsContext& operator=(GraphicsContext&& other)
			{
				MoveFrom(other);
				return *this;
			}
			GraphicsContext& operator=(GraphicsContextHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	};
}
