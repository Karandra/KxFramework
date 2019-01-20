#pragma once
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Editor.h"

namespace Kx::DataView2
{
	enum class DateEditorStyle
	{
		None = 0,

		Spin = 1 << 0,
		Dropdown = 1 << 2,
		AllowNone = 1 << 3,
		ShowCentury = 1 << 4,
	};
}

namespace Kx::DataView2
{
	class KX_API DateEditor: public Editor, public KxWithOptions<DateEditorStyle, DateEditorStyle::Dropdown>
	{
		public:
			static bool GetValueAsDateTime(const wxAny& value, wxDateTime& dateTime);
			static wxDateTime GetValueAsDateTime(const wxAny& value)
			{
				wxDateTime dateTime;
				GetValueAsDateTime(value, dateTime);
				return dateTime;
			}

		private:
			wxDateTime m_Min = wxDefaultDateTime;
			wxDateTime m_Max = wxDefaultDateTime;

		protected:
			wxWindow* CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
			wxAny GetValue(wxWindow* control) const override;

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
}
