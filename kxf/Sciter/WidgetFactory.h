#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Utility/WithOptionalOwnership.h"

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
		private:
			inline static std::vector<Utility::WithOptionalOwnership<WidgetFactory>> ms_RegisteredFactories;

		public:
			static std::unique_ptr<Widget> NewWidget(Host& host, const Element& element, const String& className);

			template<class TFunc>
			static WidgetFactory* EnumFactories(TFunc&& func)
			{
				for (auto& factory: ms_RegisteredFactories)
				{
					if (!std::invoke(func, *factory))
					{
						return factory;
					}
				}
				return nullptr;
			}

			static void RegisterFactory(WidgetFactory& factory)
			{
				ms_RegisteredFactories.emplace_back(factory);
			}
			static void RegisterFactory(std::unique_ptr<WidgetFactory> factory)
			{
				ms_RegisteredFactories.emplace_back(std::move(factory));
			}

		private:
			String m_ClassName;

		public:
			WidgetFactory(String className)
				:m_ClassName(std::move(className))
			{
			}
			virtual ~WidgetFactory() = default;

		public:
			String GetClassName() const
			{
				return m_ClassName;
			}
			bool IsStandardClass() const
			{
				return m_ClassName.StartsWith(wxS("kxf."), nullptr, StringOpFlag::FirstMatchOnly);
			}

		public:
			virtual std::unique_ptr<Widget> CreateWidget(Host& host, const Element& element, const String& className) = 0;
			virtual String GetWidgetStylesheet() const = 0;
	};

	class KX_API StdWidgetFactory: public WidgetFactory
	{
		public:
			StdWidgetFactory(String className)
				:WidgetFactory(std::move(className.Prepend(wxS("kxf."))))
			{
			}
	};
}

namespace kxf::Sciter
{
	template<class T>
	class KX_API WidgetFactoryInstance
	{
		private:
			inline static bool ms_IsRegistered = false;

		public:
			WidgetFactoryInstance() noexcept = default;
			const WidgetFactoryInstance(const WidgetFactoryInstance&) = delete;

		protected:
			~WidgetFactoryInstance() = default;

		public:
			static T& GetInstance() noexcept(std::is_nothrow_default_constructible_v<T>)
			{
				static T factory;
				return factory;
			}

			static bool IsInstanceRegistered() noexcept
			{
				return ms_IsRegistered;
			}
			static T& RegisterInstance()
			{
				T& factory = GetInstance();
				if (!ms_IsRegistered)
				{
					WidgetFactory::RegisterFactory(factory);
					AppendMasterCSS(factory.GetWidgetStylesheet());

					ms_IsRegistered = true;
				}
				return factory;
			}

		public:
			WidgetFactoryInstance& operator=(const WidgetFactoryInstance&) = delete;
	};
}
