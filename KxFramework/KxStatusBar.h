#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/UI/WindowRefreshScheduler.h"

class KX_API KxStatusBar: public wxSystemThemedControl<KxFramework::UI::WindowRefreshScheduler<wxStatusBar>>
{
	private:
		void SetFieldsCount(int count, const int* widths) override;

	public:
		static const long DefaultStyle = wxSTB_DEFAULT_STYLE;

		KxStatusBar() = default;
		KxStatusBar(wxWindow* parent,
					wxWindowID id,
					int fieldCount = 1,
					long style = DefaultStyle
		)
		{
			Create(parent, id, fieldCount, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					int fieldCount = 1,
					long style = DefaultStyle
		);

	public:
		void SetStatusWidth(int width, int index);
		void SetStatusWidths(const std::vector<int>& widths)
		{
			wxStatusBar::SetStatusWidths((int)widths.size(), widths.data());
		}
		
		virtual void SetFieldsCount(int count)
		{
			wxStatusBar::SetFieldsCount(count);
		}
		virtual void SetFieldsCount(const std::vector<int>& widths)
		{
			wxStatusBar::SetFieldsCount((int)std::min(widths.size(), (size_t)GetFieldsCount()), widths.data());
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStatusBar);
};
