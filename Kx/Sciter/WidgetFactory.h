#pragma once
#include "Kx/Sciter/Common.h"

namespace KxFramework::Sciter
{
	class Host;
	class Widget;
	class Element;
}

namespace KxFramework::Sciter
{
	class KX_API WidgetFactory
	{
		public:
			static std::unique_ptr<Widget> NewWidget(Host& host, const Element& element, const wxString& className);
			static WidgetFactory* GetFirstFactory();

			template<class TFunc>
			static WidgetFactory* EnumFactories(TFunc&& func)
			{
				for (WidgetFactory* factory = GetFirstFactory(); factory; factory = factory->GetNextFactory())
				{
					if (!func(*factory))
					{
						return factory;
					}
				}
				return nullptr;
			}

		private:
			wxString m_ClassName;
			WidgetFactory* m_NextFactory = nullptr;

		public:
			WidgetFactory(const wxString& className);
			virtual ~WidgetFactory() = default;

		public:
			wxString GetClassName() const
			{
				return m_ClassName;
			}
			WidgetFactory* GetNextFactory() const
			{
				return m_NextFactory;
			}
			
		public:
			virtual std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element, const wxString& className) = 0;
	};
}
