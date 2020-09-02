#pragma once
#include "Common.h"
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
			void OnAttached() override;

		public:
			NativeTextBoxWidget(Host& host, WidgetFactory& factory, const Element& element)
				:NativeWidget(host, factory, element)
			{
			}
			NativeTextBoxWidget(const NativeTextBoxWidget&) = delete;

		public:
			void ChangeValue(const wxString& value) override;

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
