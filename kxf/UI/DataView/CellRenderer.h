#pragma once
#include "Common.h"
#include "CellState.h"
#include "CellAttributes.h"
#include "ToolTip.h"
#include "kxf/General/Any.h"

namespace kxf
{
	class IGraphicsContext;
	class IGraphicsRenderer;
	class WidgetMouseEvent;
}
namespace kxf::DataView
{
	class CellRenderer;
}
namespace kxf::WXUI::DataView
{
	class View;
	class MainWindow;
	class MaxWidthCalculator;
}

namespace kxf
{
	class KX_API IDataViewCellRenderer: public RTTI::Interface<IDataViewCellRenderer>
	{
		KxRTTI_DeclareIID(IDataViewCellRenderer, {0xa9538cec, 0x83d5, 0x46b9, {0x9a, 0x9c, 0x1, 0x49, 0x86, 0x67, 0x21, 0x5e}});

		friend class DataView::CellRenderer;

		public:
			enum class MarkupMode
			{
				Disabled = 0,
				TextOnly,
				WithMnemonics
			};

			struct DrawInfo final
			{
				Rect CellRect;
				DataView::CellState State;
				DataView::CellAttributes Attributes;
				EllipsizeMode EllipsizeMode = EllipsizeMode::End;
				MarkupMode MarkupMode = MarkupMode::Disabled;
				IGraphicsContext* GraphicsContext = nullptr;
				const WidgetMouseEvent* MouseEvent = nullptr;
			};

		protected:
			virtual void DrawBackground(const DataView::Node& node, const DataView::Column& column, const DrawInfo& drawInfo)
			{
			}
			virtual void DrawContent(const DataView::Node& node, const DataView::Column& column, const DrawInfo& drawInfo) = 0;

			virtual Any OnActivate(const DataView::Node& node, const DataView::Column& column, const DrawInfo& drawInfo)
			{
				return {};
			}
			virtual Size GetCellSize(const DataView::Node& node, const DataView::Column& column, const DrawInfo& drawInfo) const
			{
				return {0, 0};
			}

		public:
			virtual String GetDisplayText(const Any& value) const = 0;
			virtual bool SetDisplayValue(Any value) = 0;

			virtual DataView::ToolTip CreateToolTip() const
			{
				return {};
			}
			virtual FlagSet<Alignment> GetEffectiveAlignment(const DataView::Node& node, const DataView::Column& column, FlagSet<Alignment> alignment) const;
			virtual bool IsActivatable() const
			{
				return false;
			}
	};
}

namespace kxf::DataView
{
	class KX_API CellRenderer final
	{
		friend class WXUI::DataView::MainWindow;
		friend class WXUI::DataView::MaxWidthCalculator;

		private:
			std::shared_ptr<IDataViewCellRenderer> m_CellRenderer;

			IDataViewCellRenderer::DrawInfo m_DrawInfo;
			bool m_IsViewEnabled = false;
			bool m_IsViewFocused = false;

			FlagSet<Alignment> m_Alignment = Alignment::Invalid;
			const Node* m_Node = nullptr;
			const Column* m_Column = nullptr;

		private:
			bool CanDraw() const
			{
				return m_CellRenderer && m_DrawInfo.GraphicsContext != nullptr;
			}

			bool BeginCellRendering(const Node& node, const Column& column, IGraphicsContext& gc)
			{
				m_Node = &node;
				m_Column = &column;
				m_DrawInfo.GraphicsContext = &gc;

				return CanDraw();
			}
			void EndCellRendering()
			{
				m_Node = nullptr;
				m_Column = nullptr;
				m_DrawInfo.GraphicsContext = nullptr;
			}

			void BeginCellSetup(const Node& node, const Column& column, IGraphicsContext* gc = nullptr)
			{
				m_Node = &node;
				m_Column = &column;
				m_DrawInfo.GraphicsContext = gc;
			}
			void EndCellSetup()
			{
				m_Node = nullptr;
				m_Column = nullptr;
				m_DrawInfo.GraphicsContext = nullptr;
			}

			void SetupCellDisplayValue();
			const CellAttributes& SetupCellAttributes(CellState cellState);

			void DrawCellBackground(const Rect& cellRect, CellState cellState, bool noUserBackground = false);
			std::pair<Size, Rect> DrawCellContent(const Rect& cellRect, CellState cellState);
			void OnActivateCell(Node& node, const Rect& cellRect, const WidgetMouseEvent* mouseEvent = nullptr);

			const CellAttributes& GetAttributes() const
			{
				return m_DrawInfo.Attributes;
			}
			IGraphicsContext& GetGraphicsContext() const
			{
				return *m_DrawInfo.GraphicsContext;
			}

		public:
			CellRenderer(std::shared_ptr<IDataViewCellRenderer> cellRenerer = nullptr)
				:m_CellRenderer(std::move(cellRenerer))
			{
			}
			~CellRenderer() = default;

		public:
			IDataViewWidget& GetOwningWdget() const;
			const Column& GetColumn() const
			{
				return *m_Column;
			}
			const Node& GetNode() const
			{
				return *m_Node;
			}

			bool IsActivatable() const
			{
				return m_CellRenderer && m_CellRenderer->IsActivatable();
			}

			void SetAlignment(FlagSet<Alignment> alignment)
			{
				m_Alignment = alignment;
			}
			void SetEllipsizeMode(EllipsizeMode mode)
			{
				m_DrawInfo.EllipsizeMode = mode;
			}

			void EnableMarkup(bool enable = true)
			{
				m_DrawInfo.MarkupMode = enable ? IDataViewCellRenderer::MarkupMode::TextOnly : IDataViewCellRenderer::MarkupMode::Disabled;
			}
			void EnableMarkupWithMnemonics(bool enable = true)
			{
				m_DrawInfo.MarkupMode = enable ? IDataViewCellRenderer::MarkupMode::WithMnemonics : IDataViewCellRenderer::MarkupMode::Disabled;
			}

			FlagSet<Alignment> GetEffectiveAlignment() const
			{
				if (m_CellRenderer)
				{
					return m_CellRenderer->GetEffectiveAlignment(*m_Node, *m_Column, m_Alignment);
				}
				return m_Alignment;
			}
			Size GetEffectiveCellSize() const
			{
				if (m_CellRenderer)
				{
					return m_CellRenderer->GetCellSize(*m_Node, *m_Column, m_DrawInfo);
				}
				return {0, 0};
			}
	};
}
