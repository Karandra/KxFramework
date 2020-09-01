#pragma once
#include "NativeWidget.h"
#include "kxf/UI/Controls/TextBox.h"

namespace kxf::Sciter
{
	class KX_API NativeTextBoxWidget: public NativeWidget, public UI::TextBox
	{
		protected:
			// NativeWidget
			wxWindow& GetNativeWindow() override
			{
				return *this;
			}

			// Widget
			void OnAttached() override
			{
				Element node = GetElement();
				if (TextBox::Create(GetAttachmentParent(), wxID_NONE, node.GetValue()))
				{
					NativeWidget::OnAttached();
					node.SetValue({});
				}
			}

		public:
			NativeTextBoxWidget(Host& host, WidgetFactory& factory, const Element& element)
				:NativeWidget(host, factory, element)
			{
			}
			NativeTextBoxWidget(const NativeTextBoxWidget&) = delete;

		public:
			NativeTextBoxWidget& operator=(const NativeTextBoxWidget&) = delete;
	};
}

namespace kxf::Sciter
{
	class KX_API NativeTextBoxWidgetFactory: public StdWidgetFactory, public WidgetFactoryInstance<NativeTextBoxWidgetFactory>
	{
		public:
			NativeTextBoxWidgetFactory()
				:StdWidgetFactory(wxS("NativeTextBox"))
			{
			}

		public:
			std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element, const String& className) override
			{
				return std::make_unique<NativeTextBoxWidget>(host, *this, element);
			}
			String GetWidgetStylesheet() const override;
	};
}
