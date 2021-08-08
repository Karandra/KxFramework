#pragma once
#include "Common.h"
#include "../IImageComboBoxWidget.h"
#include "../ITextWidget.h"
#include "../Private/WxTextEntryWrapper.h"

namespace kxf::WXUI
{
	class ImageComboBox;
}

namespace kxf::Widgets
{
	class KX_API ImageComboBox: public RTTI::Implementation<ImageComboBox, Private::BasicWxWidget<ImageComboBox, WXUI::ImageComboBox, IImageComboBoxWidget>, ITextEntry>
	{
		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		private:
			WXUI::Private::WxTextEntryWrapper m_TextEntryWrapper;

		public:
			ImageComboBox();
			~ImageComboBox();

		public:
			// IWidget
			bool CreateWidget(std::shared_ptr<IWidget> parent, const String& label = {}, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) override;

			// IComboBoxWidget
			void ShowDropdown() override;
			void DismissDropdown() override;

			using IComboBoxWidget::AddItem;
			size_t InsertItem(size_t index, const String& label, void* data = nullptr) override;
			void RemoveItem(size_t index) override;
			void ClearItems() override;

			String GetItemLabel(size_t index) const override;
			void SetItemLabel(size_t index, const String& label) override;

			void* GetItemData(size_t index) const override;
			void SetItemData(size_t index, void* data) override;

			size_t GetItemCount() const override;
			void SetVisibleItemCount(size_t count) override;

			size_t GetSelectedItem() const override;
			void SetSelectedItem(size_t index) override;

			// IImageComboBoxWidget
			size_t InsertItem(size_t index, const String& label, const BitmapImage& image, void* data = nullptr) override;
			using IImageComboBoxWidget::AddItem;

			BitmapImage GetItemImage(size_t index) const override;
			void SetItemImage(size_t index, const BitmapImage& image) override;

			// ITextEntry
			bool CanCut() const override
			{
				return m_TextEntryWrapper.CanCut();
			}
			bool CanCopy() const override
			{
				return m_TextEntryWrapper.CanCopy();
			}
			bool CanPaste() const override
			{
				return m_TextEntryWrapper.CanPaste();
			}
			bool CanUndo() const override
			{
				return m_TextEntryWrapper.CanUndo();
			}
			bool CanRedo() const override
			{
				return m_TextEntryWrapper.CanRedo();
			}

			void Cut() override
			{
				m_TextEntryWrapper.Cut();
			}
			void Copy() override
			{
				m_TextEntryWrapper.Undo();
			}
			void Paste() override
			{
				m_TextEntryWrapper.Undo();
			}
			void Undo() override
			{
				m_TextEntryWrapper.Undo();
			}
			void Redo() override
			{
				m_TextEntryWrapper.Redo();
			}

			void ClearText() override
			{
				m_TextEntryWrapper.ClearText();
			}
			bool IsTextEmpty() const override
			{
				return m_TextEntryWrapper.IsTextEmpty();
			}

			bool IsEditable() const override
			{
				return m_TextEntryWrapper.IsEditable();
			}
			void SetEditable(bool isEditable = true) override
			{
				m_TextEntryWrapper.SetEditable(isEditable);
			}

			bool IsMultiline() const override
			{
				return false;
			}
			void SetMultiline(bool isMultiline = true) override
			{
			}

			bool IsModified() const override
			{
				return m_TextEntryWrapper.IsModified();
			}
			void SetModified(bool isModified = true) override
			{
				m_TextEntryWrapper.SetModified(isModified);
			}

			size_t GetLengthLimit() const override
			{
				return m_TextEntryWrapper.GetLengthLimit();
			}
			void SetLengthLimit(size_t limit) override
			{
				m_TextEntryWrapper.SetLengthLimit(limit);
			}

			size_t GetTabWidth() const override
			{
				return m_TextEntryWrapper.GetTabWidth();
			}
			void SetTabWidth(size_t width) override
			{
				m_TextEntryWrapper.SetTabWidth(width);
			}

			size_t GetInsertionPoint() const override
			{
				return m_TextEntryWrapper.GetInsertionPoint();
			}
			void SetInsertionPoint(size_t pos) override
			{
				m_TextEntryWrapper.SetInsertionPoint(pos);
			}

			void SelectRange(size_t from, size_t to) override
			{
				m_TextEntryWrapper.SelectRange(from, to);
			}
			std::pair<size_t, size_t> GetSelectionRange() const override
			{
				return m_TextEntryWrapper.GetSelectionRange();
			}
			String GetRange(size_t from, size_t to) const override
			{
				return m_TextEntryWrapper.GetRange(from, to);
			}
			void RemoveRange(size_t from, size_t to) override
			{
				m_TextEntryWrapper.RemoveRange(from, to);
			}
			void ReplaceRange(size_t from, size_t to, const String& text) override
			{
				m_TextEntryWrapper.ReplaceRange(from, to, text);
			}

			size_t GetTextLength() const override
			{
				return m_TextEntryWrapper.GetTextLength();
			}
			String GetText() const override
			{
				return m_TextEntryWrapper.GetText();
			}
			void SetText(const String& text) override
			{
				m_TextEntryWrapper.SetText(text);
			}
			void AppendText(const String& text) override
			{
				m_TextEntryWrapper.AppendText(text);
			}

			String GetHint() const override
			{
				return m_TextEntryWrapper.GetHint();
			}
			void SetHint(const String& hint) override
			{
				m_TextEntryWrapper.SetHint(hint);
			}

			size_t GetRowCount() const override
			{
				return 1;
			}
			size_t GetColumnCount() const override
			{
				return m_TextEntryWrapper.GetTextLength();
			}
			size_t GetRowLength(size_t index) const override
			{
				return index == 0 ? m_TextEntryWrapper.GetTextLength() : 0;
			}

			UniChar GetPasswordMask() const
			{
				return {};
			}
			void SetPasswordMask(const UniChar& c) const
			{
			}
			void RemovePasswordMask() const
			{
			}
			using ITextEntry::SetPasswordMask;
	};
}
