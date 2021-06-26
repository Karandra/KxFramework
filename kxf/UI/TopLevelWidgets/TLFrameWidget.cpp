#include "KxfPCH.h"
#include "TLFrameWidget.h"
#include "WXUI/Frame.h"

namespace kxf
{
	TLFrameWidget::TLFrameWidget()
		:BasicWidget(std::make_unique<WXUI::Frame>(*this))
	{
	}
	TLFrameWidget::~TLFrameWidget() = default;

	bool TLFrameWidget::CreateWidget(IWidget* parent, const String& text, Point pos, Size size)
	{
		auto frame = Get();
		if (frame->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size))
		{
			frame->SetStatusBarPane(-1);
			frame->Center(wxBOTH);

			return true;
		}
		return false;
	}
}
