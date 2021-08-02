#pragma once
#include "Common.h"
#include "WxTextEntryWrapper.h"
class wxTextCtrl;

namespace kxf::WXUI::Private
{
	class KX_API WxTextCtrlWrapper: public WxTextEntryWrapper
	{
		protected:
			wxTextCtrl* m_TextCtrl = nullptr;
			UniChar m_PasswordChar;

		public:
			WxTextCtrlWrapper() = default;

		public:
			void Initialize(wxTextCtrl& textCtrl) noexcept;

		public:
			// WxTextEntryWrapper
			size_t GetLengthLimit() const;
			void SetLengthLimit(size_t maxLength);

			size_t GetTabWidth() const;
			void SetTabWidth(size_t width);

			String GetHint() const;
			void SetHint(const String& hint);

			// WxTextCtrlWrapper
			bool IsMultiline() const;
			void SetMultiline(bool isMultiline = true);

			size_t GetRowCount() const;
			size_t GetColumnCount() const;
			size_t GetRowLength(size_t index) const;

			UniChar GetPasswordMask() const;
			void SetPasswordMask(const UniChar& c) const;
			void RemovePasswordMask() const;

			void EnsurePositionVisible(size_t pos);
			Point MapPositionToCoordinates(size_t pos) const;
			size_t MapCoordinatesToPosition(const Point& point) const;

			Point MapPositionToRowColumn(size_t pos) const;
			size_t MapRowColumnToPosition(const Point& rowColumn) const;
	};
}
