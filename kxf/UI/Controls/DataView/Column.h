#pragma once
#include "Common.h"
#include "Editor.h"
#include "Renderer.h"
#include "ColumnID.h"
#include <kxf/RTTI.hpp>
#include <wx/headercol.h>

namespace kxf::UI::DataView
{
	class View;
	class MainWindow;
	class HeaderCtrl;
	class Renderer;
	class Editor;
}

namespace kxf::UI::DataView
{
	class KX_API Column;

	class KX_API NativeColumn: public wxSettableHeaderColumn
	{
		private:
			Column& m_Column;

		public:
			NativeColumn(Column& column)
				:m_Column(column)
			{
			}
	
		public:
			const Column& GetColumn() const
			{
				return m_Column;
			}
			Column& GetColumn()
			{
				return m_Column;
			}

		public:
			wxString GetTitle() const override;
			void SetTitle(const wxString& title) override;

			wxBitmap GetBitmap() const override;
			void SetBitmap(const wxBitmap& bitmap) override;
			
			int GetWidth() const override;
			void SetWidth(int width) override;
			
			int GetMinWidth() const override;
			void SetMinWidth(int minWidth) override;
			
			wxAlignment GetAlignment() const override;
			void SetAlignment(wxAlignment alignment) override;

			int GetFlags() const override;
			void SetFlags(int flags) override;

			bool IsSortKey() const override;
			bool IsSortOrderAscending() const override;
			void SetSortOrder(bool isAscending) override;
	};
}

namespace kxf::UI::DataView
{
	class KX_API Column: public RTTI::Interface<Column>, public wxClientDataContainer
	{
		KxRTTI_DeclareIID(Column, {0x95e43f36, 0x4b4a, 0x4d43, {0xa9, 0x8, 0xab, 0x1e, 0x25, 0x42, 0x9, 0xfa}});

		friend class KX_API View;
		friend class KX_API MainWindow;
		friend class KX_API HeaderCtrl;
		friend class KX_API HeaderCtrl2;
		friend class KX_API NativeColumn;

		public:
			using Vector = std::vector<std::unique_ptr<Column>>;
			using RefVector = std::vector<Column*>;

		private:
			static int GetAbsMinColumnWidth();
			static int GetAbsMaxColumnWidth();

		private:
			NativeColumn m_NativeColumn;
			View* m_View = nullptr;
			std::unique_ptr<Renderer> m_Renderer;
			std::unique_ptr<Editor> m_Editor;

			size_t m_Index = std::numeric_limits<size_t>::max();
			size_t m_DisplayIndex = std::numeric_limits<size_t>::max();
			ColumnID m_ID;

			GDIBitmap m_Bitmap;
			String m_Title;
			FlagSet<Alignment> m_TitleAlignment = Alignment::Invalid;
			bool m_IsChecked = false;

			FlagSet<ColumnStyle> m_Style = ColumnStyle::Default;
			ColumnWidth m_Width;
			int m_MinWidth = 0;
			int m_BestWidth = 0;
			wxRecursionGuardFlag m_BestWidthRG = 0;

			bool m_IsDirty = true;
			bool m_IsVisible = true;
			bool m_IsSorted = false;
			bool m_IsSortedAscending = true;

		private:
			void SetView(View* view)
			{
				m_View = view;
			}
			void SetSortOrder(bool ascending);

		protected:
			void UpdateDisplay();
			bool IsDirty() const
			{
				return m_IsDirty;
			}
			void MarkDirty(bool value = true);
			
			void AssignIndex(size_t value)
			{
				m_Index = value;
			}
			void AssignDisplayIndex(size_t value)
			{
				m_DisplayIndex = value;
			}
			void AssignPhysicalDisplayIndex(size_t value)
			{
				m_DisplayIndex = value + GetInvisibleColumnsBefore();
			}

			void AssignWidth(ColumnWidth width);
			void AssignVisible(bool value)
			{
				m_IsVisible = value;
			}

			ColumnStyle GetStyleFlags() const
			{
				return m_Style.GetValue();
			}
			void SetStyleFlags(ColumnStyle style)
			{
				m_Style.SetValue(style);
			}

			const NativeColumn& GetNativeColumn() const
			{
				return m_NativeColumn;
			}
			NativeColumn& GetNativeColumn()
			{
				return m_NativeColumn;
			}

		public:
			Column()
				:m_NativeColumn(*this)
			{
			}
			Column(const wxString& title, ColumnID id, Renderer* renderer = nullptr)
				:m_NativeColumn(*this), m_Title(title), m_Renderer(renderer), m_ID(id)
			{
			}
			Column(const wxBitmap& bitmap, ColumnID id, Renderer* renderer = nullptr)
				:m_NativeColumn(*this), m_Bitmap(bitmap), m_Renderer(renderer), m_ID(id)
			{
			}
			Column(const wxBitmap& bitmap, const wxString& title, ColumnID id, Renderer* renderer = nullptr)
				:m_NativeColumn(*this), m_Bitmap(bitmap), m_Title(title), m_Renderer(renderer), m_ID(id)
			{
			}
			
			Column(const wxString& title, ColumnID id, ColumnWidth width, ColumnStyle style)
				:m_NativeColumn(*this), m_Title(title), m_ID(id), m_Width(width), m_Style(style)
			{
			}
			Column(const wxBitmap& bitmap, ColumnID id, ColumnWidth width, ColumnStyle style)
				:m_NativeColumn(*this), m_Bitmap(bitmap), m_ID(id), m_Width(width), m_Style(style)
			{
			}

			virtual ~Column();

		public:
			View* GetView() const
			{
				return m_View;
			}
			MainWindow* GetMainWindow() const;

			Renderer& GetRenderer() const;
			void AssignRenderer(Renderer* renderer);

			Editor* GetEditor() const;
			void AssignEditor(Editor* editor);

		public:
			size_t GetIndex() const
			{
				return m_Index;
			}
			size_t GetDisplayIndex() const
			{
				return m_DisplayIndex;
			}
			size_t GetPhysicalDisplayIndex() const;
			void SetDisplayIndex(size_t newPosition);
			void SetPhysicalDisplayIndex(size_t newPosition);
			size_t GetInvisibleColumnsBefore() const;

			template<class T> T GetID() const
			{
				return m_ID.GetValue<T>();
			}
			ColumnID GetID() const
			{
				return m_ID;
			}
			void SetID(ColumnID id)
			{
				m_ID = id;
			}
			
			bool HasBitmap() const
			{
				return !m_Bitmap.IsNull();
			}
			GDIBitmap GetBitmap() const
			{
				return m_Bitmap;
			}
			void SetBitmap(const GDIBitmap& bitmap)
			{
				m_Bitmap = bitmap;
				UpdateDisplay();
			}

			bool HasTitle() const
			{
				return m_Title.IsEmpty();
			}
			String GetTitle() const
			{
				return m_Title;
			}
			void SetTitle(String title)
			{
				m_Title = std::move(title);
				UpdateDisplay();
			}

			bool HasCheckBox() const
			{
				return m_Style.Contains(ColumnStyle::CheckBox);
			}
			bool IsChecked() const
			{
				return m_IsChecked;
			}
			void SetChecked(bool value = true)
			{
				m_IsChecked = value;
				UpdateDisplay();
			}

			ColumnWidth GetWidthDescriptor() const
			{
				return m_Width;
			}
			int GetWidth() const;
			void SetWidth(ColumnWidth width);

			int GetMinWidth() const;
			void SetMinWidth(int width);

			bool HasBestWidth() const
			{
				return m_BestWidth > 0;
			}
			int GetBestWidth() const
			{
				return m_BestWidth;
			}
			void SetBestWidth(int width)
			{
				m_BestWidth = std::clamp(width, GetAbsMinColumnWidth(), GetAbsMaxColumnWidth());
			}
			void InvalidateBestWidth()
			{
				m_BestWidth = 0;
				MarkDirty();
			}
			int CalcBestSize();

			int GetTitleWidth() const;
			FlagSet<Alignment> GetTitleAlignment() const
			{
				return m_TitleAlignment;
			}
			void SetTitleAlignment(FlagSet<Alignment> alignment)
			{
				m_TitleAlignment = alignment;
				UpdateDisplay();
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

			void SortAscending();
			void SortDescending();
			void ToggleSortOrder();
			void ResetSorting();

			const auto& GetStyle() const
			{
				return m_Style;
			}
			auto& GetStyle()
			{
				return m_Style;
			}

			bool IsSortable() const
			{
				return m_Style.Contains(ColumnStyle::Sort);
			}
			void SetSortable(bool value)
			{
				m_Style.Mod(ColumnStyle::Sort, value);
			}
			
			bool IsMoveable() const
			{
				return m_Style.Contains(ColumnStyle::Move);
			}
			void SetMoveable(bool value)
			{
				m_Style.Mod(ColumnStyle::Move, value);
			}
			
			bool IsSizeable() const
			{
				return m_Style.Contains(ColumnStyle::Size);
			}
			void SetSizeable(bool value)
			{
				m_Style.Mod(ColumnStyle::Size, value);
			}

			bool HasDropdown() const
			{
				return m_Style.Contains(ColumnStyle::Dropdown);
			}
			void ShowDropdown(bool value)
			{
				m_Style.Mod(ColumnStyle::Dropdown, value);
			}

			bool IsExposed(int& width) const;
			bool IsExposed() const
			{
				int width = 0;
				return IsExposed(width);
			}
			bool IsVisible() const
			{
				return m_IsVisible;
			}
			void SetVisible(bool value)
			{
				AssignVisible(value);
				UpdateDisplay();
			}
			void Refresh() const;
			void FitInside();
			bool FitContent();

			bool IsRenderable() const
			{
				return m_Renderer != nullptr;
			}
			bool IsEditable() const
			{
				return m_Editor != nullptr;
			}
			bool IsActivatable() const;

			bool IsExpander() const;
			bool IsHotTracked() const;
			bool IsCurrent() const;

			bool IsFirst() const;
			bool IsDisplayedFirst() const;

			bool IsLast() const;
			bool IsDisplayedLast() const;

			Rect GetRect() const;
			Rect GetDropdownRect() const;
	};
}
