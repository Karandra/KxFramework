#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewEditor.h"
#include "KxFramework/KxWithOptions.h"
#include <wx/datectrl.h>

enum KxDataViewDateOptions
{
	KxDVR_DATE_NONE = 0,

	KxDVR_DATE_SPIN = wxDP_SPIN,
	KxDVR_DATE_DROPDOWN = wxDP_DROPDOWN,
	KxDVR_DATE_ALLOW_NONE = wxDP_ALLOWNONE,
	KxDVR_DATE_SHOW_CENTURY = wxDP_SHOWCENTURY,

	KxDVR_DATE_DEFAULT = KxDVR_DATE_DROPDOWN,
};

class KX_API KxDataViewDateEditor:
	public KxDataViewEditor,
	public KxWithOptions<KxDataViewDateOptions, KxDVR_DATE_DEFAULT>
{
	public:
		static bool GetValueAsDateTime(const wxAny& value, wxDateTime& dateTime);

	private:
		wxDateTime m_Min = wxDefaultDateTime;
		wxDateTime m_Max = wxDefaultDateTime;

	protected:
		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;

		wxDateTime GetMin() const
		{
			return m_Min;
		}
		wxDateTime GetMax() const
		{
			return m_Max;
		}
		void SetRange(const wxDateTime& min, const wxDateTime& max)
		{
			if (min <= max)
			{
				m_Min = min;
				m_Max = max;
			}
			else
			{
				m_Min = max;
				m_Max = min;
			}
		}
};
