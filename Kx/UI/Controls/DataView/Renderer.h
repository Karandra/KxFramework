#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttribute.h"
#include "RenderEngine.h"
#include "ToolTip.h"
#include <wx/dc.h>
#include <wx/graphics.h>
#include <wx/dcgraph.h>

namespace KxFramework::UI::DataView
{
	class Node;
	class View;
	class Model;
	class Column;
	class MainWindow;
	class RenderEngine;
}

namespace KxFramework::UI::DataView
{
	class KX_API Renderer
	{
		friend class Model;
		friend class Column;
		friend class MainWindow;
		friend class MaxWidthCalculator;
		friend class RenderEngine;

		public:
			enum class ProgressState
			{
				Normal,
				Paused,
				Error,
				Partial,
			};
			enum class MarkupMode
			{
				Disabled = 0,
				TextOnly,
				WithMnemonics
			};

		private:
			wxAlignment m_Alignment = wxALIGN_INVALID;
			wxEllipsizeMode m_EllipsizeMode = wxELLIPSIZE_END;
			MarkupMode m_MarkupMode = MarkupMode::Disabled;
			CellAttribute m_Attributes;
			
			Rect m_PaintRect;
			const Node* m_Node = nullptr;
			Column* m_Column = nullptr;

			wxGCDC* m_GraphicsDC = nullptr;
			wxDC* m_RegularDC = nullptr;
			bool m_AlwaysUseGC = false;

		private:
			void BeginCellRendering(const Node& node, Column& column, wxGCDC& graphicsDC, wxDC* regularDC = nullptr)
			{
				m_Node = &node;
				m_Column = &column;
				m_GraphicsDC = &graphicsDC;
				m_RegularDC = regularDC;
			}
			void EndCellRendering()
			{
				m_Node = nullptr;
				m_Column = nullptr;
				m_GraphicsDC = nullptr;
				m_RegularDC = nullptr;
			}
			bool IsNullRenderer() const;

			void BeginCellSetup(const Node& node, Column& column)
			{
				m_Node = &node;
				m_Column = &column;
			}
			void EndCellSetup()
			{
				m_Node = nullptr;
				m_Column = nullptr;
			}
			void SetupCellValue();
			void SetupCellAttributes(CellState cellState);
			
			void CallDrawCellBackground(const Rect& cellRect, CellState cellState, bool noUserBackground = false);
			void CallDrawCellContent(const Rect& cellRect, CellState cellState, bool alwaysUseGC = false);
			void CallOnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent = nullptr);

		protected:
			const CellAttribute& GetAttributes() const
			{
				return m_Attributes;
			}
			Rect GetPaintRect() const
			{
				return m_PaintRect;
			}

			virtual bool HasActivator() const
			{
				return false;
			}
			virtual wxAny OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent = nullptr)
			{
				return {};
			}
			virtual bool SetValue(const wxAny& value) = 0;
			virtual ToolTip CreateToolTip() const
			{
				return {};
			}

			virtual bool HasSolidBackground() const;
			virtual bool HasSpecialBackground() const;
			virtual void DrawCellBackground(const Rect& cellRect, CellState cellState)
			{
			}
			virtual void DrawCellContent(const Rect& cellRect, CellState cellState) = 0;
			virtual Size GetCellSize() const;

			template<class TValue> TValue FromAnyUsing(const wxAny& value) const
			{
				TValue rendererValue;
				rendererValue.FromAny(value);
				return rendererValue;
			}

		public:
			bool HasRegularDC() const
			{
				return m_RegularDC != nullptr;
			}
			wxDC& GetRegularDC() const
			{
				return *m_RegularDC;
			}

			bool HasGraphicsDC() const
			{
				return m_GraphicsDC != nullptr;
			}
			wxGCDC& GetGraphicsDC() const
			{
				return *m_GraphicsDC;
			}
			wxGraphicsContext& GetGraphicsContext() const
			{
				return *m_GraphicsDC->GetGraphicsContext();
			}

			RenderEngine GetRenderEngine() const
			{
				return RenderEngine(const_cast<Renderer&>(*this), m_AlwaysUseGC);
			}
			virtual wxString GetTextValue(const wxAny& value) const = 0;

		public:
			Renderer(int alignment = wxALIGN_INVALID)
				:m_Alignment(static_cast<wxAlignment>(alignment))
			{
			}
			virtual ~Renderer() = default;

		public:
			MainWindow* GetMainWindow() const;
			View* GetView() const;
			Column* GetColumn() const
			{
				return m_Column;
			}
			const Node* GetNode() const
			{
				return m_Node;
			}

			virtual wxAlignment GetEffectiveAlignment() const;
			wxAlignment GetAlignment() const
			{
				return m_Alignment;
			}
			void SetAlignment(wxAlignment alignment)
			{
				m_Alignment = alignment;
			}
			void SetAlignment(int alignment)
			{
				m_Alignment = static_cast<wxAlignment>(alignment);
			}

			wxEllipsizeMode GetEllipsizeMode() const
			{
				return m_EllipsizeMode;
			}
			void SetEllipsizeMode(wxEllipsizeMode mode)
			{
				m_EllipsizeMode = mode;
			}

			bool IsActivatable() const
			{
				return HasActivator();
			}

			bool IsMarkupEnabled() const
			{
				return m_MarkupMode != MarkupMode::Disabled;
			}
			bool IsTextMarkupEnabled() const
			{
				return m_MarkupMode == MarkupMode::TextOnly;
			}
			bool IsMarkupWithMnemonicsEnabled() const
			{
				return m_MarkupMode == MarkupMode::WithMnemonics;
			}
			
			void EnableMarkup(bool enable = true)
			{
				m_MarkupMode = enable ? MarkupMode::TextOnly : MarkupMode::Disabled;
			}
			void EnableMarkupWithMnemonics(bool enable = true)
			{
				m_MarkupMode = enable ? MarkupMode::WithMnemonics : MarkupMode::Disabled;
			}
	};
}
