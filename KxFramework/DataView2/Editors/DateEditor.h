#pragma once
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Editor.h"
#include "Kx/General/WithOptions.h"

namespace KxDataView2
{
	enum class DateEditorOptions
	{
		None = 0,

		Spin = 1 << 0,
		Dropdown = 1 << 2,
		AllowNone = 1 << 3,
		ShowCentury = 1 << 4,
	};
}

namespace KxDataView2
{
	class KX_API DateTimeValue: public KxFramework::WithOptions<DateEditorOptions, DateEditorOptions::None>
	{
		public:
			using Options = DateEditorOptions;

		protected:
			wxDateTime m_Value;
			wxDateTime m_RangeLower;
			wxDateTime m_RangeUpper;

		public:
			DateTimeValue(const wxDateTime& value = {})
				:m_Value(value)
			{
			}
			DateTimeValue(const wxDateTime& value, const wxDateTime& lower, const wxDateTime& upper)
				:m_Value(value)
			{
				SetDateTimeRange(lower, upper);
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				*this = {};
			}

			bool HasDateTime() const
			{
				return m_Value.IsValid();
			}
			wxDateTime GetDateTime() const
			{
				return m_Value;
			}
			void SetDateTime(const wxDateTime& value)
			{
				m_Value = value;
			}
			void ClearDateTime()
			{
				m_Value = {};
			}
			
			bool HasDateRange() const
			{
				return m_RangeLower.IsValid() && m_RangeUpper.IsValid();
			}
			std::pair<wxDateTime, wxDateTime> GetDateTimeRange() const
			{
				return {m_RangeLower, m_RangeUpper};
			}
			void SetDateTimeRange(const wxDateTime& lower, const wxDateTime& upper)
			{
				m_RangeLower = lower;
				m_RangeUpper = upper;

				if (m_RangeLower > m_RangeUpper)
				{
					std::swap(m_RangeLower, m_RangeUpper);
				}
			}
			void ClearDateTimeRange()
			{
				m_RangeLower = {};
				m_RangeUpper = {};
			}
	};
}

namespace KxDataView2
{
	class KX_API DateEditor: public Editor
	{
		protected:
			wxWindow* CreateControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
			wxAny GetValue(wxWindow* control) const override;
	};
}
