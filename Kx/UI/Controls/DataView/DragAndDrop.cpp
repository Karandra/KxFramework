#include "stdafx.h"
#include "DragAndDrop.h"
#include "MainWindow.h"
#include "Kx/UI/Windows/SplashWindow.h"
#include "Kx/Utility/Common.h"
#include <wx/dragimag.h>

namespace KxFramework::UI::DataView
{
	bool DnDInfo::DoCheckOperation(const wxDataFormat& format, DNDOpType desiredType) const
	{
		if (const DNDOperationInfo* info = GetOperationInfo(format))
		{
			return Utility::HasFlag(info->GetType(), desiredType);
		}
		return false;
	}
	DnDInfo::Result DnDInfo::DoChangeOperation(const wxDataFormat& format, const DNDOperationInfo& info)
	{
		auto it = m_DataFormats.find(format);
		if (it != m_DataFormats.end())
		{
			if (info.IsOK() && format != wxDF_INVALID)
			{
				it->second.Combine(info);
				return Result::OperationChanged;
			}
			else
			{
				m_DataFormats.erase(it);
				return Result::OperationRemoved;
			}
		}
		else if (info.IsOK() && format != wxDF_INVALID)
		{
			m_DataFormats.insert_or_assign(format, info);
			return Result::OperationAdded;
		}
		return Result::None;
	}
}

namespace KxFramework::UI::DataView
{
	void DropSource::OnScroll(wxMouseEvent& event)
	{
		if (m_MainWindow)
		{
			View* view = m_MainWindow->GetView();

			int rateX = 0;
			int rateY = 0;
			view->GetScrollPixelsPerUnit(&rateX, &rateY);
			wxPoint startPos = view->GetViewStart();

			wxCoord value = -event.GetWheelRotation();
			if (event.GetWheelAxis() == wxMOUSE_WHEEL_VERTICAL)
			{
				view->Scroll(wxDefaultCoord, startPos.y + (float)value / (rateY != 0 ? rateY : 1));
			}
			else
			{
				view->Scroll(startPos.x + (float)value / (rateX != 0 ? rateX : 1), wxDefaultCoord);
			}
		}
		event.Skip();
	}
	bool DropSource::GiveFeedback(wxDragResult effect)
	{
		wxPoint mousePos = wxGetMousePosition();
		if (m_DragImage == nullptr)
		{
			wxPoint linePos(0, m_MainWindow->GetRowStart(m_Row));

			m_MainWindow->GetView()->CalcUnscrolledPosition(0, linePos.y, nullptr, &linePos.y);
			m_MainWindow->ClientToScreen(&linePos.x, &linePos.y);

			int rowIndent = 0;
			m_HintBitmap = m_MainWindow->CreateItemBitmap(m_Row, rowIndent);

			m_Distance.x = (mousePos.x - linePos.x) - rowIndent;
			m_Distance.y = mousePos.y - linePos.y;
			m_HintPosition = GetHintPosition(mousePos);

			m_DragImage = new SplashWindow(m_MainWindow, m_HintBitmap, {}, EnumClass::Combine<SplashWindowStyle>(TopLevelWindowStyle::StayOnTop));
			m_DragImage->SetTransparent(225);
			m_DragImage->Update();
			m_DragImage->Show();
		}
		else
		{
			m_HintPosition = GetHintPosition(mousePos);
			m_DragImage->Move(m_HintPosition);
		}
		return false;
	}

	wxPoint DropSource::GetHintPosition(const wxPoint& mousePos) const
	{
		return wxPoint(mousePos.x - m_Distance.x, mousePos.y + 5);
	}

	DropSource::DropSource(MainWindow* mainWindow, Row row)
		:wxDropSource(mainWindow), m_MainWindow(mainWindow), m_Row(row), m_Distance(0, 0)
	{
	}
	DropSource::~DropSource()
	{
		m_HintPosition = wxDefaultPosition;
		if (m_DragImage)
		{
			m_DragImage->Destroy();
		}
	}
}

namespace KxFramework::UI::DataView
{
	wxDataFormat DropTarget::GetReceivedFormat() const
	{
		return const_cast<DropTarget*>(this)->GetMatchingPair();
	}
	bool DropTarget::IsFormatSupported(const wxDataFormat& format, wxDataObjectSimple*& dataObject) const
	{
		if (format != wxDF_INVALID)
		{
			dataObject = m_MainWindow->m_DragDropDataObject->GetObject(format);
			return dataObject != nullptr;
		}
		return false;
	}

	wxDragResult DropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult dragResult)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); IsFormatSupported(format, dataObject))
		{
			return m_MainWindow->OnDragOver(*dataObject, wxPoint(x, y), dragResult);
		}
		return wxDragNone;
	}
	bool DropTarget::OnDrop(wxCoord x, wxCoord y)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); IsFormatSupported(format, dataObject))
		{
			return m_MainWindow->TestDropPossible(*dataObject, wxPoint(x, y));
		}
		return false;
	}
	wxDragResult DropTarget::OnData(wxCoord x, wxCoord y, wxDragResult dragResult)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); IsFormatSupported(format, dataObject))
		{
			GetData();
			return m_MainWindow->OnDropData(*dataObject, wxPoint(x, y), dragResult);
		}
		return wxDragNone;
	}

	wxDragResult DropTarget::OnEnter(wxCoord x, wxCoord y, wxDragResult dragResult)
	{
		wxDataObjectSimple* dataObject = nullptr;
		if (wxDataFormat format = GetReceivedFormat(); GetData() && IsFormatSupported(format, dataObject))
		{
			return m_MainWindow->OnDragDropEnter(*dataObject, wxPoint(x, y), dragResult);
		}
		return wxDragNone;
	}
	void DropTarget::OnLeave()
	{
		m_MainWindow->OnDragDropLeave();
	}

	DropTarget::DropTarget(wxDataObject* dataObject, MainWindow* mainWindow)
		:wxDropTarget(dataObject), m_MainWindow(mainWindow)
	{
	}
}
