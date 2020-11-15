#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttribute.h"
#include "RenderEngine.h"
#include "ToolTip.h"
#include <wx/graphics.h>

namespace kxf::UI::DataView
{
	class Node;
	class View;
	class Model;
	class Column;
	class MainWindow;
	class RenderEngine;
}

namespace kxf::UI::DataView
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
			FlagSet<Alignment> m_Alignment = Alignment::Invalid;
			EllipsizeMode m_EllipsizeMode = EllipsizeMode::End;
			MarkupMode m_MarkupMode = MarkupMode::Disabled;
			CellAttribute m_Attributes;

			Rect m_PaintRect;
			const Node* m_Node = nullptr;
			Column* m_Column = nullptr;

			IGraphicsContext* m_GC = nullptr;

		private:
			void BeginCellRendering(const Node& node, Column& column, IGraphicsContext& gc)
			{
				m_Node = &node;
				m_Column = &column;
				m_GC = &gc;
			}
			void EndCellRendering()
			{
				m_Node = nullptr;
				m_Column = nullptr;
				m_GC = nullptr;
			}
			bool IsNullRenderer() const;
			bool CanDraw() const
			{
				return m_GC != nullptr;
			}

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
			void CallDrawCellContent(const Rect& cellRect, CellState cellState);
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

			template<class TValue>
			TValue FromAnyUsing(const wxAny& value) const
			{
				TValue rendererValue;
				rendererValue.FromAny(value);
				return rendererValue;
			}

		public:
			IGraphicsContext& GetGraphicsContext() const
			{
				return *m_GC;
			}
			RenderEngine GetRenderEngine() const
			{
				return RenderEngine(const_cast<Renderer&>(*this));
			}
			virtual String GetTextValue(const wxAny& value) const = 0;

		public:
			Renderer(FlagSet<Alignment> alignment = Alignment::Invalid)
				:m_Alignment(alignment)
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

			virtual FlagSet<Alignment> GetEffectiveAlignment() const;
			FlagSet<Alignment> GetAlignment() const
			{
				return m_Alignment;
			}
			void SetAlignment(FlagSet<Alignment> alignment)
			{
				m_Alignment = alignment;
			}

			EllipsizeMode GetEllipsizeMode() const
			{
				return m_EllipsizeMode;
			}
			void SetEllipsizeMode(EllipsizeMode mode)
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
