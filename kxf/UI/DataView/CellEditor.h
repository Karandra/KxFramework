#pragma once
#include "Common.h"
#include "kxf/General/Any.h"
#include "kxf/EventSystem/EvtHandler.h"

namespace kxf
{
	class IdleEvent;
	class WidgetKeyEvent;
	class WidgetMouseEvent;
	class WidgetFocusEvent;
	class WidgetTextEvent;
}
namespace kxf::DataView
{
	class CellEditor;
}
namespace kxf::WXUI::DataView
{
	class View;
	class MainWindow;
}

namespace kxf
{
	class KX_API IDataViewCellEditor: public RTTI::Interface<IDataViewCellEditor>
	{
		KxRTTI_DeclareIID(IDataViewCellEditor, {0x8a08c334, 0xebce, 0x4ccd, {0x8c, 0xa5, 0xe7, 0x78, 0xd, 0xc9, 0xbf, 0x53}});

		friend class DataView::CellEditor;

		public:
			struct EditorInfo final
			{
				DataView::Node& Node;
				DataView::Column& Column;

				Rect CellRect;
				IWidget* Widget = nullptr;
				bool IsEditable = true;
			};

		protected:
			virtual std::shared_ptr<IWidget> CreateWidget(std::shared_ptr<IWidget> parent, const EditorInfo& editorInfo) = 0;
			virtual std::shared_ptr<IEvtHandler> CreateWidgetHandler()
			{
				return nullptr;
			}
			virtual void DestroyWidget(IWidget& widget)
			{
			}

		public:
			virtual bool BeginEdit(const EditorInfo& editorInfo, Any value) = 0;
			virtual void EndEdit(const EditorInfo& editorInfo) = 0;
			virtual void CancelEdit(const EditorInfo& editorInfo) = 0;

			virtual Any GetValue(const EditorInfo& editorInfo) const = 0;
	};
}

namespace kxf::DataView
{
	class KX_API CellEditor final
	{
		friend class MainWindow;
		friend class CellEditorWidgetHandler;

		private:
			struct EditorParameters final
			{
				Rect CellRect;
				IWidget* Widget = nullptr;
				bool IsEditable = true;
			};

		private:
			std::shared_ptr<IDataViewCellEditor> m_CellEditor;
			std::shared_ptr<IWidget> m_Widget;
			std::shared_ptr<IEvtHandler> m_WidgetEvtHandler;

			EditorParameters m_Parameters;
			Node* m_Node = nullptr;
			Column* m_Column = nullptr;
			bool m_IsEditCanceled = false;
			bool m_IsEditFinished = false;

		private:
			bool IsNull() const noexcept
			{
				return m_CellEditor == nullptr;
			}
			bool IsEditCanceled() const
			{
				return m_IsEditCanceled;
			}
			bool IsEditFinished() const
			{
				return m_IsEditFinished;
			}

			void OnBeginEdit(Node& node, Column& column)
			{
				OnEndEdit();

				m_Node = &node;
				m_Column = &column;
				m_IsEditCanceled = false;
				m_IsEditFinished = false;
			}
			void OnEndEdit()
			{
				DestroyWidget();

				m_Parameters = {};
				m_Node = nullptr;
				m_Column = nullptr;
			}
			void DestroyWidget();

			IDataViewCellEditor::EditorInfo CreateParemeters() const
			{
				IDataViewCellEditor::EditorInfo info = {*m_Node, *m_Column};
				info.Widget = m_Parameters.Widget;
				info.CellRect = m_Parameters.CellRect;
				info.IsEditable = m_Parameters.IsEditable;

				return info;
			}

		public:
			CellEditor(std::shared_ptr<IDataViewCellEditor> cellEditor = nullptr)
				:m_CellEditor(std::move(cellEditor))
			{
			}
			~CellEditor()
			{
				DestroyWidget();
			}

		public:
			IDataViewWidget& GetOwningWidget() const;

			std::shared_ptr<IWidget> GetWidget() const
			{
				return m_Widget;
			}
			Column& GetColumn() const
			{
				return *m_Column;
			}
			Node& GetNode() const
			{
				return *m_Node;
			}

			bool IsActive() const
			{
				return m_Column && m_Node;
			}
			bool BeginEdit(Node& node, Column& column, const Rect& cellRect);
			bool EndEdit();
			void CancelEdit();

			bool IsEditable() const
			{
				return m_Parameters.IsEditable;
			}
			void SetEditable(bool isEditable = true)
			{
				m_Parameters.IsEditable = isEditable;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}

namespace kxf::DataView
{
	class KX_API CellEditorWidgetHandler: public EvtHandler
	{
		private:
			CellEditor& m_Editor;

			bool m_IsFinished = false;
			bool m_SetFocusOnIdle = false;

		private:
			void OnIdle(IdleEvent& event);
			void OnChar(WidgetKeyEvent& event);
			void OnTextCommit(WidgetTextEvent& event);
			void OnKillFocus(WidgetFocusEvent& event);
			void OnMouseMove(WidgetMouseEvent& event);

		protected:
			CellEditor& GetEditor() const
			{
				return m_Editor;
			}
			IWidget& GetWidget() const
			{
				return *m_Editor.m_Widget;
			}

		public:
			CellEditorWidgetHandler(CellEditor& editor);

		public:
			void SetFocusOnIdle(bool focus = true)
			{
				m_SetFocusOnIdle = focus;
			}
	};
}
