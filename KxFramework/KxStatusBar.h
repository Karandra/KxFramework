#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxStatusBar: public wxSystemThemedControl<wxStatusBar>
{
	private:
		virtual void SetFieldsCount(int count, const int* widths) override;

	public:
		static const long DefaultStyle = wxSTB_DEFAULT_STYLE;
		static const long DefaultHeight = 23;
		static const long DefaultFiledsCount = 1;

		KxStatusBar() {}
		KxStatusBar(wxWindow* parent,
					wxWindowID winid,
					int fieldsCount = DefaultFiledsCount,
					long style = DefaultStyle
		)
		{
			Create(parent, winid, fieldsCount, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID winid,
					int fieldsCount = DefaultFiledsCount,
					long style = DefaultStyle
		);

	public:
		void SetStatusWidth(int width, int index);
		void SetStatusWidths(const KxIntVector& widths)
		{
			wxStatusBar::SetStatusWidths((int)widths.size(), widths.data());
		}
		
		virtual void SetFieldsCount(int count)
		{
			wxStatusBar::SetFieldsCount(count);
		}
		virtual void SetFieldsCount(const KxIntVector& widths)
		{
			wxStatusBar::SetFieldsCount((int)std::min(widths.size(), (size_t)GetFieldsCount()), widths.data());
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStatusBar);
};
