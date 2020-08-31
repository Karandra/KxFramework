#pragma once
#include "kxf/Sciter/Widget.h"
#include "kxf/Sciter/WidgetFactory.h"

namespace kxf::Sciter
{
	class KX_API NativeWidget: public Widget
	{
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
