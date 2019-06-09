#include "KxStdAfx.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "HeaderCtrl.h"
#include "Renderer.h"
#include "Editor.h"
#include "KxFramework/KxUtility.h"

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
		KxUtility::ModFlagRef(flags, wxCOL_RESIZABLE, m_Column.IsSizeable());
		KxUtility::ModFlagRef(flags, wxCOL_SORTABLE, m_Column.IsSortable());
		KxUtility::ModFlagRef(flags, wxCOL_REORDERABLE, m_Column.IsMoveable());
		
		// Use raw visibility flag. Native control does some weird things
		// if the flag value is calculated using column width.
		KxUtility::ModFlagRef(flags, wxCOL_HIDDEN, !m_Column.IsVisible());

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
			m_View->OnColumnChange(m_Index);
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

	Column::~Column()
	{
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
		if (width.IsSpecialValue())
		{
			m_Width = width;
		}
		else
		{
			m_Width = std::clamp<int>(width, m_MinWidth, GetAbsMaxColumnWidth());
		}
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
			return m_View->m_HeaderArea->GetDropdownRect(m_Index);
		}
		return {};
	}
}
