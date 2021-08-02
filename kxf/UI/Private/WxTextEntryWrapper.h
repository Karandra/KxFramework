#pragma once
#include "Common.h"
class wxTextEntry;

namespace kxf::WXUI::Private
{
	class KX_API WxTextEntryWrapper
	{
		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		protected:
			wxTextEntry* m_TextEntry = nullptr;

		private:
			size_t m_LengthLimit = npos;
			size_t m_TabWidth = 0;
			bool m_IsModified = false;

		public:
			WxTextEntryWrapper() = default;

		public:
			void Initialize(wxTextEntry& textEntry) noexcept;

		public:
			bool CanCut() const;
			bool CanCopy() const;
			bool CanPaste() const;
			bool CanUndo() const;
			bool CanRedo() const;

			void Cut();
			void Copy();
			void Paste();
			void Undo();
			void Redo();

			void ClearText();
			bool IsTextEmpty() const;

			bool IsEditable() const;
			void SetEditable(bool isEditable = true);

			bool IsModified() const;
			void SetModified(bool isModified = true);

			size_t GetLengthLimit() const;
			void SetLengthLimit(size_t limit);

			size_t GetTabWidth() const;
			void SetTabWidth(size_t width);

			size_t GetInsertionPoint() const;
			void SetInsertionPoint(size_t pos);

			void SelectRange(size_t from, size_t to);
			std::pair<size_t, size_t> GetSelectionRange() const;
			String GetRange(size_t from, size_t to) const;
			void RemoveRange(size_t from, size_t to);
			void ReplaceRange(size_t from, size_t to, const String& text);

			String GetText() const;
			void SetText(const String& text);
			void AppendText(const String& text);

			String GetHint() const;
			void SetHint(const String& hint);
	};
}
