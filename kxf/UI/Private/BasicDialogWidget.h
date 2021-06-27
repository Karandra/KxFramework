#pragma once
#include "Common.h"
#include "BasicWindowWidget.h"
#include "../IDalogWidget.h"
class wxDialog;

namespace kxf::Private
{
	class KX_API BasicDialogWidgetBase: public BasicWindowWidgetBase
	{
		public:
			BasicDialogWidgetBase(IWidget& widget) noexcept
				:BasicWindowWidgetBase(widget)
			{
			}
			~BasicDialogWidgetBase() = default;

		public:
			// BasicWxWidgetBase
			wxDialog* GetWxWindow() const noexcept;

		public:
			// BasicDialogWidgetBase
			bool IsModal() const;
			void EndModal(int retCode);
			int ShowModal();
			int GetReturnCode() const;

			int GetAffirmativeID() const;
			void SetAffirmativeID(int id);

			int GetEscapeID() const;
			void SetEscapeID(int id);
	};
}

namespace kxf::Private
{
	template<class TDerived, class TWindow, class TInterface, class TWindowImpl = BasicDialogWidgetBase>
	class BasicDialogWidget: public BasicWindowWidget<TDerived, TWindow, TInterface, TWindowImpl>
	{
		protected:
			using BasicWindowWidget<TDerived, TWindow, TInterface, TWindowImpl>::GetImpl;

		public:
			bool IsModal() const override
			{
				return GetImpl().IsModal();
			}
			void EndModal(int retCode) override
			{
				GetImpl().EndModal(retCode);
			}
			int ShowModal() override
			{
				return GetImpl().ShowModal();
			}
			int GetReturnCode() const override
			{
				return GetImpl().GetReturnCode();
			}

			int GetAffirmativeID() const override
			{
				return GetImpl().GetAffirmativeID();
			}
			void SetAffirmativeID(int id) override
			{
				GetImpl().SetAffirmativeID(id);
			}

			int GetEscapeID() const override
			{
				return GetImpl().GetEscapeID();
			}
			void SetEscapeID(int id) override
			{
				GetImpl().SetEscapeID(id);
			}
	};
}
