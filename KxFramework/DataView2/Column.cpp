#include "KxStdAfx.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "Renderer.h"
#include "Editor.h"
#include "KxFramework/KxUtility.h"

namespace Kx::DataView2
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
		return m_Column.GetWidth();
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
		KxUtility::ModFlagRef(flags, wxCOL_HIDDEN, !m_Column.m_IsVisible);

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

namespace Kx::DataView2
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

namespace Kx::DataView2
{
	void Column::SetSortOrder(bool ascending)
	{
		if (m_View)
		{

			// If this column isn't sorted already, mark it as sorted
			if (!m_IsSorted)
			{
				// Now set this one as the new sort column.
				m_View->UseColumnForSorting(m_Index);
				m_IsSorted = true;
			}
			m_IsSortedAscending = ascending;

			// Call this directly instead of using UpdateDisplay() as we already have
			// the column index, no need to look it up again.
			m_View->OnColumnChange(m_Index);
		}
	}
	void Column::UpdateDisplay()
	{
		m_View->OnColumnChange(m_Index);
	}
	void Column::MarkDirty(bool value)
	{
		m_IsDirty = value;
		if (value)
		{
			m_View->m_IsColumnsDirty = true;
		}
	}

	Column::~Column()
	{
		delete m_Renderer;
		delete m_Editor;
	}

	Renderer& Column::GetRenderer() const
	{
		if (m_Renderer)
		{
			return *m_Renderer;
		}
		return GetView()->GetMainWindow()->GetNullRenderer();
	}
	Editor* Column::GetEditor() const
	{
		return m_Editor;
	}

	int Column::GetWidth() const
	{
		switch (static_cast<ColumnWidth>(m_Width))
		{
			case ColumnWidth::Default:
			{
				return m_View->FromDIP(wxSize(80, 0)).GetWidth();
			}
			case ColumnWidth::AutoSize:
			{
				return const_cast<Column&>(*this).CalcBestSize();
			}
		};
		return static_cast<int>(m_Width);
	}
	void Column::SetWidth(int width)
	{
		switch (static_cast<ColumnWidth>(width))
		{
			case ColumnWidth::Default:
			case ColumnWidth::AutoSize:
			{
				m_Width = static_cast<ColumnWidth>(width);
				UpdateDisplay();
				return;
			}
		};

		m_Width = static_cast<ColumnWidth>(std::clamp(width, m_MinWidth, GetAbsMaxColumnWidth()));
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
		m_IsSorted = false;

		if (m_View)
		{
			m_View->DontUseColumnForSorting(m_Index);
		}
		UpdateDisplay();
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
		return m_Index == m_View->GetColumnCount();
	}
	bool Column::IsDisplayedLast() const
	{
		return m_DisplayIndex == m_View->GetVisibleColumnCount();
	}
}
