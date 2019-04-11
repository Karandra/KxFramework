#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
class KX_API KxDataViewCtrl;
class KX_API KxDataViewMainWindow;
class KX_API KxDataViewHeaderCtrl;
class KX_API KxDataViewRenderer;
class KX_API KxDataViewEditor;
class KX_API KxDataViewModel;

class KX_API KxDataViewColumn:
	private wxSettableHeaderColumn,
	public KxWithOptions<KxDataViewColumnFlags, KxDV_COL_NONE>,
	public wxClientDataContainer
{
	friend class KX_API KxDataViewHeaderCtrl;
	friend class KX_API KxDataViewMainWindow;
	friend class KX_API KxDataViewCtrl;

	public:
		using Vector = std::vector<KxDataViewColumn*>;

	private:
		KxDataViewCtrl* m_DataView = nullptr;
		KxDataViewRenderer* m_Renderer = nullptr;
		KxDataViewEditor* m_Editor = nullptr;
		bool m_ShouldDeleteEditor = true;
		bool m_DynamicEditor = false;

		wxBitmap m_Bitmap;
		wxString m_Title;
		int m_ID = -1;

		int m_Width = KxCOL_WIDTH_DEFAULT;
		int m_MinWidth = 0;
		wxAlignment m_Alignment = wxALIGN_INVALID;

		bool m_IsSorted = false;
		bool m_IsSortedAscending = true;

	private:
		void Init();

	private:
		bool IsSortOrderAscending() const override
		{
			return IsSortedAscending();
		}
		bool IsSortKey() const override
		{
			return IsSorted();
		}
		void UnsetAsSortKey() override
		{
			ResetSorting();
		}
		void SetSortOrder(bool ascending) override;

	protected:
		bool DoIsExposed(int& width) const;
		wxSettableHeaderColumn* GetAsSettableHeaderColumn()
		{
			return this;
		}
		void UpdateDisplay();
		void SetOwner(KxDataViewCtrl* owner)
		{
			m_DataView = owner;
		}

		// Editor
		void DeleteEditorIfNeeded();
		void DoSetEditor(KxDataViewEditor* editor);

	public:
		KxDataViewColumn()
		{
		}
		KxDataViewColumn(const wxString& title, KxDataViewRenderer* renderer, int id)
			:m_Title(title), m_Renderer(renderer), m_ID(id)
		{
			Init();
		}
		KxDataViewColumn(const wxBitmap& bitmap, KxDataViewRenderer* renderer, int id)
			:m_Bitmap(bitmap), m_Renderer(renderer), m_ID(id)
		{
			Init();
		}
		KxDataViewColumn(const wxBitmap& bitmap, const wxString& title, KxDataViewRenderer* renderer, int id)
			:m_Bitmap(bitmap), m_Title(title), m_Renderer(renderer), m_ID(id)
		{
			Init();
		}
		virtual ~KxDataViewColumn();

	public:
		KxDataViewCtrl* GetOwner() const
		{
			return m_DataView;
		}
		KxDataViewRenderer* GetRenderer() const
		{
			return m_Renderer;
		}

		bool HasEditor() const
		{
			return m_Editor != nullptr;
		}
		KxDataViewEditor* GetEditor() const
		{
			return m_Editor;
		}
		void SetEditor(KxDataViewEditor* editor)
		{
			DoSetEditor(editor);
			m_ShouldDeleteEditor = false;
		}
		void AssignEditor(KxDataViewEditor* editor)
		{
			DoSetEditor(editor);
			m_ShouldDeleteEditor = true;
		}
		void RemoveEditor()
		{
			DoSetEditor(nullptr);
			m_ShouldDeleteEditor = false;
		}
		void UseDynamicEditor(bool use)
		{
			m_DynamicEditor = use;
		}

		bool IsEditable() const;
		bool IsActivatable() const;

	public:
		int GetID() const
		{
			return m_ID;
		}
		void SetID(int id)
		{
			m_ID = id;
		}
		
		int GetFlags() const override
		{
			return GetOptionsValue();
		}
		void SetFlags(int flags) override
		{
			SetOptionsValue(flags);
			UpdateDisplay();
		}

		bool HasBitmap() const
		{
			return m_Bitmap.IsOk();
		}
		wxBitmap GetBitmap() const override
		{
			return m_Bitmap;
		}
		void SetBitmap(const wxBitmap& bitmap) override
		{
			m_Bitmap = bitmap;
			UpdateDisplay();
		}

		bool HasTitle() const
		{
			return m_Title.IsEmpty();
		}
		wxString GetTitle() const override
		{
			return m_Title;
		}
		void SetTitle(const wxString& title) override
		{
			m_Title = title;
			UpdateDisplay();
		}

		bool UsesAutomaticWidth() const
		{
			return m_Width == KxCOL_WIDTH_AUTOSIZE;
		}
		int GetWidth() const override;
		void SetWidth(int width) override;
		
		int GetMinWidth() const override
		{
			return m_MinWidth;
		}
		void SetMinWidth(int minWidth) override
		{
			m_MinWidth = std::abs(minWidth);
			UpdateDisplay();
		}

		void SetAlignment(wxAlignment align) override
		{
			m_Alignment = align;
			UpdateDisplay();
		}
		wxAlignment GetAlignment() const override
		{
			return m_Alignment;
		}

		bool IsSorted() const
		{
			return m_IsSorted;
		}
		bool IsSortedAscending() const
		{
			return m_IsSortedAscending;
		}
		bool IsSortedDescending() const
		{
			return !IsSortedAscending();
		}

		void SortAscending()
		{
			SetSortOrder(true);
		}
		void SortDescending()
		{
			SetSortOrder(false);
		}
		void ResetSorting();

		bool IsSortable() const
		{
			return wxSettableHeaderColumn::IsSortable();
		}
		bool IsReorderable() const
		{
			return wxSettableHeaderColumn::IsReorderable();
		}
		bool IsResizeable() const
		{
			return wxSettableHeaderColumn::IsResizeable();
		}
		void SetSortable(bool value)
		{
			wxSettableHeaderColumn::SetSortable(value);
		}
		void SetReorderable(bool value)
		{
			wxSettableHeaderColumn::SetReorderable(value);
		}
		void SetResizeable(bool value)
		{
			wxSettableHeaderColumn::SetResizeable(value);
		}

		bool IsExposed(int& width) const
		{
			width = 0;
			return DoIsExposed(width);
		}
		bool IsVisible() const
		{
			return wxSettableHeaderColumn::IsShown();
		}
		void SetVisible(bool value)
		{
			wxSettableHeaderColumn::SetHidden(!value);
		}

		bool IsHotTracked() const;
		bool IsCurrent() const;
		bool IsFirst() const;
		bool IsLast() const;
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxDataViewColumnPart
{
	friend class KxDataViewColumn;

	private:
		KxDataViewColumn* m_Column = nullptr;

	private:
		void SetOwner(KxDataViewColumn* column)
		{
			m_Column = column;
		}

	public:
		KxDataViewColumnPart(KxDataViewColumn* column)
			:m_Column(column)
		{
		}
		virtual ~KxDataViewColumnPart()
		{
		}

	public:
		KxDataViewMainWindow* GetMainWindow() const;
		KxDataViewCtrl* GetView() const;
		KxDataViewModel* GetModel() const;
		KxDataViewColumn* GetColumn() const;
		KxDataViewRenderer* GetRenderer() const;
};
