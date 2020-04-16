#include "stdafx.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "HeaderCtrl.h"
#include "Renderer.h"
#include "Editor.h"
#include "Kx/Utility/Common.h"

using namespace KxFramework;

namespace KxDataView2
{
	wxString NativeColumn::GetTitle() const
	{
		return m_Column.GetTitle();
	}
	void NativeColumn::SetTitle(const wxString& title)
	{
		m_Column.SetTitle(title);
	}

	wxBitmap NativeColumn::GetBitmap() const
	{
		return m_Column.GetBitmap();
	}
	void NativeColumn::SetBitmap(const wxBitmap& bitmap)
	{
		m_Column.SetBitmap(bitmap);
	}

	int NativeColumn::GetWidth() const
	{
		return m_Column.GetWidthDescriptor();
	}
	void NativeColumn::SetWidth(int width)
	{
		m_Column.SetWidth(width);
	}

	int NativeColumn::GetMinWidth() const
	{
		return m_Column.GetMinWidth();
	}
	void NativeColumn::SetMinWidth(int minWidth)
	{
		m_Column.SetMinWidth(minWidth);
	}

	wxAlignment NativeColumn::GetAlignment() const
	{
		return m_Column.GetTitleAlignment();
	}
	void NativeColumn::SetAlignment(wxAlignment alignment)
	{
		m_Column.SetTitleAlignment(alignment);
	}

	int NativeColumn::GetFlags() const
	{
		int flags = 0;
		Utility::ModFlagRef(flags, wxCOL_RESIZABLE, m_Column.IsSizeable());
		Utility::ModFlagRef(flags, wxCOL_SORTABLE, m_Column.IsSortable());
		Utility::ModFlagRef(flags, wxCOL_REORDERABLE, m_Column.IsMoveable());
		
		// Use raw visibility flag. Native control does some weird things
		// if the flag value is calculated using column width.
		Utility::ModFlagRef(flags, wxCOL_HIDDEN, !m_Column.IsVisible());

		return flags;
	}
	void NativeColumn::SetFlags(int flags)
	{
		m_Column.SetSizeable(flags & wxCOL_RESIZABLE);
		m_Column.SetSortable(flags & wxCOL_SORTABLE);
		m_Column.SetMoveable(flags & wxCOL_REORDERABLE);
		m_Column.SetVisible(!(flags & wxCOL_HIDDEN));
	}

	bool NativeColumn::IsSortKey() const
	{
		return m_Column.IsSorted();
	}
	bool NativeColumn::IsSortOrderAscending() const
	{
		return m_Column.IsSortedAscending();
	}
	void NativeColumn::SetSortOrder(bool isAscending)
	{
		isAscending ? m_Column.SortAscending() : m_Column.SortDescending();
	}
}

namespace KxDataView2
{
	int Column::GetAbsMinColumnWidth()
	{
		return 0;
	}
	int Column::GetAbsMaxColumnWidth()
	{
		return wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	}
}

namespace KxDataView2
{
	void Column::SetSortOrder(bool ascending)
	{
		// If this column isn't sorted already, mark it as sorted
		m_IsSorted = true;
		m_IsSortedAscending = ascending;

		UpdateDisplay();
	}
	void Column::UpdateDisplay()
	{
		if (m_View)
		{
			m_View->OnColumnChange(*this);
		}
	}
	void Column::MarkDirty(bool value)
	{
		m_IsDirty = value;
		if (m_View && value)
		{
			m_View->m_ColumnsDirty = true;
		}
	}

	void Column::AssignWidth(ColumnWidth width)
	{
		if (width.IsSpecialValue())
		{
			m_Width = width;
		}
		else
		{
			m_Width = std::clamp<int>(width, m_MinWidth, GetAbsMaxColumnWidth());
		}
	}

	Column::~Column()
	{
	}

	MainWindow* Column::GetMainWindow() const
	{
		return m_View ? m_View->GetMainWindow() : nullptr;
	}

	Renderer& Column::GetRenderer() const
	{
		if (m_Renderer)
		{
			return *m_Renderer;
		}
		return GetView()->GetMainWindow()->GetNullRenderer();
	}
	void Column::AssignRenderer(Renderer* renderer)
	{
		m_Renderer.reset(renderer);
	}

	Editor* Column::GetEditor() const
	{
		return m_Editor.get();
	}
	void Column::AssignEditor(Editor* editor)
	{
		m_Editor.reset(editor);
	}

	size_t Column::GetPhysicalDisplayIndex() const
	{
		if (IsVisible())
		{
			auto& columns = m_View->m_Columns;

			size_t physicalIndex = 0;
			for (size_t i = 0; i < columns.size(); i++)
			{
				Column* column = m_View->GetColumnDisplayedAt(i);
				if (column == this)
				{
					return physicalIndex;
				}
				else if (column->IsVisible())
				{
					physicalIndex++;
				}
			}
		}
		return std::numeric_limits<size_t>::max();
	}
	void Column::SetDisplayIndex(size_t newPosition)
	{
		if (m_View)
		{
			m_View->MoveColumn(*this, newPosition);
			UpdateDisplay();
		}
		else
		{
			AssignDisplayIndex(newPosition);
		}
	}
	void Column::SetPhysicalDisplayIndex(size_t newPosition)
	{
		if (IsVisible())
		{
			if (m_View)
			{
				m_View->MoveColumnToPhysicalIndex(*this, newPosition);
				UpdateDisplay();
			}
			else
			{
				AssignPhysicalDisplayIndex(newPosition);
			}
		}
	}
	size_t Column::GetInvisibleColumnsBefore() const
	{
		size_t invisibleBefore = 0;
		for (const auto& column: m_View->m_Columns)
		{
			if (column->GetIndex() < m_Index && !column->IsVisible())
			{
				invisibleBefore++;
			}
		}
		return invisibleBefore;
	}

	int Column::GetWidth() const
	{
		int width = m_Width;
		switch (m_Width)
		{
			case ColumnWidth::Default:
			{
				width = m_View->FromDIP(wxSize(80, 0)).GetWidth();
				break;
			}
			case ColumnWidth::AutoSize:
			{
				width = const_cast<Column&>(*this).CalcBestSize();
				break;
			}
		};
		return std::clamp(width, GetMinWidth(), std::numeric_limits<int>::max());
	}
	void Column::SetWidth(ColumnWidth width)
	{
		AssignWidth(width);
		UpdateDisplay();
	}
	int Column::CalcBestSize()
	{
		// Recursion guard for calling from user code
		if (wxRecursionGuard guard(m_BestWidthRG); !guard.IsInside())
		{
			m_View->GetMainWindow()->CalcBestColumnWidth(*this);
		}
		return m_BestWidth;
	}
	
	int Column::GetTitleWidth() const
	{
		if (m_View)
		{
			wxWindow* window = m_View;
			if (HeaderCtrl* header = m_View->GetHeaderCtrl())
			{
				window = header;
			}

			int width = window->GetTextExtent(m_Title).GetWidth();
			width += wxRendererNative::Get().GetHeaderButtonMargin(window);

			// If a bitmap is used, add space for it and 2px border
			if (const wxBitmap& bitmap = m_Bitmap; bitmap.IsOk())
			{
				width += bitmap.GetWidth() + window->FromDIP(wxSize(2, 0)).GetWidth();
			}

			return width;
		}
		return -1;
	}
	int Column::GetMinWidth() const
	{
		int width = m_MinWidth;
		if (m_View)
		{
			if (HasDropdown())
			{
				width += GetDropdownRect().GetWidth();
				width += m_View->FromDIP(wxSize(HasCheckBox() ? 1 : 2, 0)).GetWidth();
			}
			if (HasCheckBox())
			{
				width += wxRendererNative::Get().GetCheckBoxSize(m_View).GetWidth();
				width += m_View->FromDIP(wxSize(8, 0)).GetWidth();
			}
		}
		return width;
	}
	void Column::SetMinWidth(int width)
	{
		m_MinWidth = std::clamp(width, GetAbsMinColumnWidth(), GetAbsMaxColumnWidth());
		UpdateDisplay();
	}

	void Column::SortAscending()
	{
		SetSortOrder(true);
	}
	void Column::SortDescending()
	{
		SetSortOrder(false);
	}
	void Column::ToggleSortOrder()
	{
		if (m_IsSorted)
		{
			if (m_IsSortedAscending)
			{
				SortDescending();
			}
			else
			{
				SortAscending();
			}
		}
	}
	void Column::ResetSorting()
	{
		if (m_IsSorted)
		{
			m_IsSorted = false;
			UpdateDisplay();
		}
	}

	bool Column::IsExposed(int& width) const
	{
		if (IsVisible())
		{
			width = GetWidth();
			if (width > 0)
			{
				return true;
			}
		}

		width = 0;
		return false;
	}
	void Column::Refresh() const
	{
		m_View->GetMainWindow()->RefreshColumn(*this);
	}
	void Column::FitInside()
	{
		if (MainWindow* mainWindow = GetMainWindow())
		{
			mainWindow->RecalculateDisplay();
			for (auto& column: m_View->m_Columns)
			{
				column->CalcBestSize();
				if (column.get() == this)
				{
					m_Width = m_BestWidth;
				}
			}

			const int clientWidth = mainWindow->GetClientSize().GetWidth();
			const int virtualWidth = mainWindow->GetRowWidth();
			if (virtualWidth > clientWidth)
			{
				m_Width -= virtualWidth - clientWidth;
			}
			else
			{
				m_Width += clientWidth - virtualWidth;
			}

			SetWidth(m_Width);
			UpdateDisplay();
		}
	}
	bool Column::FitContent()
	{
		if (m_View)
		{
			Event event(EvtCOLUMN_HEADER_WIDTH_FIT);
			event.SetWidth(std::max({GetTitleWidth(), CalcBestSize(), GetMinWidth()}));

			GetMainWindow()->CreateEventTemplate(event, nullptr, this);
			m_View->ProcessWindowEvent(event);
			if (event.IsAllowed())
			{
				SetWidth(event.GetWidth());
				return true;
			}
		}
		return false;
	}

	bool Column::IsActivatable() const
	{
		return m_Renderer ? m_Renderer->IsActivatable() : false;
	}
	bool Column::IsExpander() const
	{
		return m_View->GetExpanderColumnOrFirstOne() == this;
	}
	bool Column::IsHotTracked() const
	{
		return m_View->GetMainWindow()->GetHotTrackColumn() == this;
	}
	bool Column::IsCurrent() const
	{
		return m_View->GetMainWindow()->GetCurrentColumn() == this;
	}
	
	bool Column::IsFirst() const
	{
		return m_Index == 0;
	}
	bool Column::IsDisplayedFirst() const
	{
		return m_DisplayIndex == 0;
	}
	
	bool Column::IsLast() const
	{
		return m_Index + 1 == m_View->GetColumnCount();
	}
	bool Column::IsDisplayedLast() const
	{
		return m_DisplayIndex + 1 == m_View->GetVisibleColumnCount();
	}

	wxRect Column::GetRect() const
	{
		int x = 0;
		for (size_t i = 0; i < m_View->GetColumnCount(); i++)
		{
			if (const Column* column = m_View->GetColumnDisplayedAt(i))
			{
				if (column == this)
				{
					break;
				}

				int width = 0;
				if (column->IsExposed(width))
				{
					x += width;
				}
			}
		}

		return {x, 0, GetWidth(), m_View->GetMainWindow()->GetClientSize().GetHeight()};
	}
	wxRect Column::GetDropdownRect() const
	{
		if (m_View && m_View->m_HeaderArea && IsVisible())
		{
			return m_View->m_HeaderArea->GetDropdownRect(*this);
		}
		return {};
	}
}
