#include "KxfPCH.h"
#include "CellEditor.h"
#include "Column.h"
#include "../Widgets/WXUI/DataView/View.h"
#include "../Widgets/WXUI/DataView/MainWindow.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/EventSystem/IdleEvent.h"
#include "kxf/UI/Events/WidgetKeyEvent.h"
#include "kxf/UI/Events/WidgetMouseEvent.h"
#include "kxf/UI/Events/WidgetFocusEvent.h"
#include "kxf/UI/Events/WidgetTextEvent.h"
#include "kxf/Utility/ScopeGuard.h"

namespace kxf::DataView
{
	void CellEditor::DestroyWidget()
	{
		// Remove our event handler first to prevent it from (recursively) calling
		// us again as it would do via a call to 'EndEdit' when the editor
		// loses focus when we hide it below.
		if (m_WidgetEvtHandler)
		{
			if (m_Widget)
			{
				m_Widget->RemoveEventHandler(*m_WidgetEvtHandler);
			}
			ICoreApplication::GetInstance()->ScheduleForDestruction(std::move(m_WidgetEvtHandler));
		}

		if (m_Widget)
		{
			// Hide the widget immediately but don't destroy it yet as there could be some pending messages for it.
			m_Widget->Hide();
			if (m_CellEditor)
			{
				m_CellEditor->DestroyWidget(*m_Widget);
			}

			// Ensure that 'DestroyWidget' is not called again for this widget.
			ICoreApplication::GetInstance()->ScheduleForDestruction(std::move(m_Widget));
		}
	}

	bool CellEditor::BeginEdit(Node& node, Column& column, const Rect& cellRect)
	{
		OnBeginEdit(node, column);
		m_Parameters.CellRect = cellRect;

		// Before doing anything we send an event asking if editing of this item is really wanted.
		auto view = m_Column->m_View;
		auto mainWindow = view->m_ClientArea;

		if (m_CellEditor && mainWindow->SendEditingStartedEvent(node, column))
		{
			if (m_Widget = m_CellEditor->CreateWidget(GetOwningWidget().QueryInterface<IWidget>(), CreateParemeters()))
			{
				m_WidgetEvtHandler = m_CellEditor->CreateWidgetHandler();
				if (!m_WidgetEvtHandler)
				{
					m_WidgetEvtHandler = std::make_shared<CellEditorWidgetHandler>(*this);
				}
				m_Widget->PushEventHandler(*m_WidgetEvtHandler);
				m_Widget->SetFocus();

				m_Parameters.Widget = m_Widget.get();
				if (m_CellEditor->BeginEdit(CreateParemeters(), node.GetCellValue(column)))
				{
					return true;
				}
			}
		}

		OnEndEdit();
		return false;
	}
	bool CellEditor::EndEdit()
	{
		if (m_Node && m_Column)
		{
			Utility::ScopeGuard atExit = [&]()
			{
				OnEndEdit();
			};

			if (m_CellEditor)
			{
				auto view = m_Column->m_View;
				auto mainWindow = view->m_ClientArea;

				// Try to get the value, normally we should succeed but if we fail, don't
				// return immediately, we still need to destroy the edit control.
				Any value = m_CellEditor->GetValue(CreateParemeters());
				DestroyWidget();
				view->SetFocus();

				if (!value.IsNull() && mainWindow->SendEditingDoneEvent(*m_Node, *m_Column, false, value))
				{
					if (m_Node->SetCellValue(*m_Column, std::move(value)))
					{
						mainWindow->OnCellChanged(*m_Node, m_Column);

						m_IsEditFinished = true;
						return true;
					}
				}
			}
		}
		return false;
	}
	void CellEditor::CancelEdit()
	{
		m_IsEditCanceled = true;
		OnEndEdit();
	}

	IDataViewWidget& CellEditor::GetOwningWidget() const
	{
		return m_Column->m_View->m_Widget;
	}
}

namespace kxf::DataView
{
	void CellEditorWidgetHandler::OnIdle(IdleEvent& event)
	{
		event.Skip();

		if (m_SetFocusOnIdle)
		{
			if (IWidget::FindFocus().get() != &GetWidget())
			{
				GetWidget().SetFocus();
			}
			m_SetFocusOnIdle = false;
		}
	}
	void CellEditorWidgetHandler::OnChar(WidgetKeyEvent& event)
	{
		switch (event.GetKeyCode())
		{
			case KeyCode::Escape:
			{
				m_IsFinished = true;
				m_Editor.CancelEdit();
				break;
			}
			case KeyCode::Enter:
			{
				if (!event.GetKeyboardState().HasAnyModifiers())
				{
					m_IsFinished = true;
					m_Editor.EndEdit();
					break;
				}

				// Ctrl/Alt/Shift-Enter is not handled specially
				[[fallthrough]];
			}
			default:
			{
				event.Skip();
			}
		};
	}
	void CellEditorWidgetHandler::OnTextCommit(WidgetTextEvent& event)
	{
		m_IsFinished = true;
		m_Editor.EndEdit();
	}
	void CellEditorWidgetHandler::OnKillFocus(WidgetFocusEvent& event)
	{
		event.Skip();

		if (!m_IsFinished)
		{
			m_IsFinished = true;
			m_Editor.EndEdit();
		}
	}
	void CellEditorWidgetHandler::OnMouseMove(WidgetMouseEvent& event)
	{
		auto& widget = GetWidget();
		const auto originalPos = event.GetPosition();

		event.SetPosition(widget.GetPosition());
		m_Editor.GetOwningWidget().ProcessEvent(event, event.GetEventID());
		event.SetPosition(originalPos);

		event.Skip();
	}

	CellEditorWidgetHandler::CellEditorWidgetHandler(CellEditor& editor)
		:m_Editor(editor)
	{
		m_IsFinished = false;
		m_SetFocusOnIdle = false;

		Bind(IdleEvent::EvtIdle, &CellEditorWidgetHandler::OnIdle, this);
		Bind(WidgetKeyEvent::EvtChar, &CellEditorWidgetHandler::OnChar, this);
		Bind(WidgetFocusEvent::EvtFocusLost, &CellEditorWidgetHandler::OnKillFocus, this);
		Bind(WidgetTextEvent::EvtCommit, &CellEditorWidgetHandler::OnTextCommit, this);

		// Causes drag events when focus is in editor and such. Disabled for now.
		//Bind(WidgetMouseEvent::EvtMove, &CellEditorWidgetHandler::OnMouseMove, this);
	}
}
