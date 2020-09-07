#pragma once
#include "kxf/Sciter/Widget.h"
#include "kxf/Sciter/WidgetFactory.h"

namespace kxf::Sciter
{
	class KX_API NativeWidget: public Widget
	{
		KxDeclareIID(NativeWidget, {0x78304789, 0x595b, 0x4fce, {0xa2, 0x34, 0x90, 0x8e, 0xd1, 0x77, 0x9c, 0xd2}});
		KxImplementQueryInterface(NativeWidget, Widget);

		private:
			wxWindow* m_NativeWindow = nullptr;

		protected:
			// NativeWidget
			virtual wxWindow& GetNativeWindow() = 0;
			wxWindow* GetAttachmentParent() const;

			// Widget
			void OnAttached() override;
			void OnDetached() override;

		public:
			NativeWidget(Host& host, WidgetFactory& factory, const Element& element)
				:Widget(host, factory, element)
			{
			}
			NativeWidget(const NativeWidget&) = delete;

		public:
			NativeWidget& operator=(const NativeWidget&) = delete;
	};
}
