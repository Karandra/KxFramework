#pragma once
#include "kxf/Sciter/Common.h"

namespace kxf::Sciter
{
	class Host;
	class Widget;
	class Element;
}

namespace kxf::Sciter
{
	class KX_API WidgetFactory
	{
		public:
			static std::unique_ptr<Widget> NewWidget(Host& host, const Element& element, const String& className);
			static WidgetFactory* GetFirstFactory();

			template<class TFunc>
			static WidgetFactory* EnumFactories(TFunc&& func)
			{
				for (WidgetFactory* factory = GetFirstFactory(); factory; factory = factory->GetNextFactory())
				{
					if (!std::invoke(func, *factory))
					{
						return factory;
					}
				}
				return nullptr;
			}

		private:
			String m_ClassName;
			WidgetFactory* m_NextFactory = nullptr;

		public:
			WidgetFactory(const String& className);
			virtual ~WidgetFactory() = default;

		public:
			String GetClassName() const
			{
				return m_ClassName;
			}
			WidgetFactory* GetNextFactory() const
			{
				return m_NextFactory;
			}
			
		public:
			virtual std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element, const String& className) = 0;
	};
}
