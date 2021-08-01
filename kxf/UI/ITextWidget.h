#pragma once
#include "Common.h"

namespace kxf
{
	class KX_API ITextWidget: public RTTI::Interface<ITextWidget>
	{
		KxRTTI_DeclareIID(ITextWidget, {0x42e05214, 0x6cf3, 0x4fd5, {0x95, 0x53, 0x20, 0xc7, 0xdc, 0x95, 0xa, 0x9}});

		public:
			static constexpr size_t npos = std::numeric_limits<size_t>::max();

		public:
			virtual ~ITextWidget() = default;

		public:
			virtual bool CanCut() const = 0;
			virtual bool CanCopy() const = 0;
			virtual bool CanPaste() const = 0;
			virtual bool CanUndo() const = 0;
			virtual bool CanRedo() const = 0;

			virtual void Cut() = 0;
			virtual void Copy() = 0;
			virtual void Paste() = 0;
			virtual void Undo() = 0;
			virtual void Redo() = 0;

			virtual void ClearText() = 0;
			virtual bool IsTextEmpty() const = 0;

			virtual bool IsEditable() const = 0;
			virtual void SetEditable(bool isEditable = true) = 0;

			virtual bool IsModified() const = 0;
			virtual void SetModified(bool isModified = true) = 0;

			virtual size_t GetLengthLimit() const = 0;
			virtual void SetLengthLimit(size_t limit) = 0;

			virtual size_t GetInsertionPoint() const = 0;
			virtual void SetInsertionPoint(size_t pos) = 0;

			virtual void SelectRange(size_t from, size_t to) = 0;
			void SelectNone()
			{
				SelectRange(0, 0);
			}
			void SelectAll()
			{
				SelectRange(0, npos);
			}

			virtual std::pair<size_t, size_t> GetSelectionRange() const = 0;
			virtual String GetRange(size_t from, size_t to) const = 0;
			virtual void RemoveRange(size_t from, size_t to) = 0;
			virtual void ReplaceRange(size_t from, size_t to, const String& text) = 0;

			bool IsSelectionRangeEmpty() const
			{
				auto [from, to] = GetSelectionRange();
				return from == to || from == npos;
			}
			void RemoveSelectedRange()
			{
				auto [from, to] = GetSelectionRange();
				RemoveRange(from, to);
			}
			void ReplaceSelectedRange(const String& text)
			{
				auto [from, to] = GetSelectionRange();
				ReplaceRange(from, to, text);
			}
			String GetSelectedRange() const
			{
				auto [from, to] = GetSelectionRange();
				return GetRange(from, to);
			}

			virtual String GetText() const = 0;
			virtual void SetText(const String& text) = 0;
			virtual void AppendText(const String& text) = 0;

			virtual String GetHint() const = 0;
			virtual void SetHint(const String& hint) = 0;
	};
}
