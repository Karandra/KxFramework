#pragma once
#include "KxFramework/KxFramework.h"

namespace KxValidator
{
	void FixIsCharOk(const wxValidator& validator, wxString& value, int& position, wxChar& character);
}

template<class T> class KxIntegerValidator: public wxIntegerValidator<T>
{
	public:
		using Base = wxIntegerValidator<T>;

	protected:
		bool IsCharOk(const wxString& value, int position, wxChar character) const override
		{
			KxValidator::FixIsCharOk(*this, const_cast<wxString&>(value), position, character);
			return Base::IsCharOk(value, position, character);
		}

	public:
		KxIntegerValidator(T* value = nullptr, int style = wxNUM_VAL_DEFAULT)
			:Base(value, style)
		{
		}

	public:
		KxIntegerValidator* Clone() const override
		{
			return new KxIntegerValidator(*this);
		}
};

template<class T> class KxFloatingPointValidator: public wxFloatingPointValidator<T>
{
	public:
		using Base = wxFloatingPointValidator<T>;

	protected:
		bool IsCharOk(const wxString& value, int position, wxChar character) const override
		{
			KxValidator::FixIsCharOk(*this, const_cast<wxString&>(value), position, character);
			return Base::IsCharOk(value, position, character);
		}

	public:
		KxFloatingPointValidator(T* value = nullptr, int style = wxNUM_VAL_DEFAULT)
			:Base(value, style)
		{
		}

	public:
		KxFloatingPointValidator* Clone() const override
		{
			return new KxFloatingPointValidator(*this);
		}
};
