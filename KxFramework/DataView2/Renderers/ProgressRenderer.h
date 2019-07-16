#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"
#include "TextRenderer.h"

namespace KxDataView2
{
	enum class ProgressHeight: int
	{
		Auto = -1,
		Fit = -2,
	};

	class KX_API ProgressValueBase
	{
		public:
			using State = ProgressState;
			using Height = ProgressHeight;

		private:
			int m_Range = 100;
			int m_Position = -1;
			State m_State = State::Normal;
			Height m_Height = Height::Auto;

		public:
			ProgressValueBase(int position = -1, State state = State::Normal)
				:m_State(state)
			{
				SetPosition(position);
			}
			ProgressValueBase(int position, int range, State state = State::Normal)
				:m_State(state)
			{
				SetRange(range);
				SetPosition(position);
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				*this = {};
			}

			int GetRange() const
			{
				return m_Range;
			}
			void SetRange(int range)
			{
				m_Range = std::clamp(range, 0, std::numeric_limits<int>::max());

				// Update position to not exceed range
				SetPosition(m_Position);
			}

			bool HasPosition() const
			{
				return m_Position >= 0;
			}
			int GetPosition() const
			{
				return m_Position;
			}
			double GetPositionRatio() const
			{
				if (m_Range != 0 && HasPosition())
				{
					return (double)m_Position / (double)m_Range;
				}
				return 0;
			}
			void SetPosition(int position)
			{
				// Allow -1 to indicate that no position has been specified
				m_Position = std::clamp(position, -1, m_Range);
			}
			void SetPosition(double pos)
			{
				if (pos < 0)
				{
					ClearPosition();
				}
				else if (pos > 1)
				{
					m_Position = m_Range;
				}
				else
				{
					m_Position = pos * m_Range;
				}
			}
			void ClearPosition()
			{
				m_Position = -1;
			}

			State GetState() const
			{
				return m_State;
			}
			void SetState(State state)
			{
				m_State = state;
			}
			void ClearState()
			{
				m_State = State::Normal;
			}
			
			template<class T = Height> T GetHeight() const
			{
				return static_cast<T>(m_Height);
			}
			void SetHeight(Height height)
			{
				m_Height = height;
			}
			void SetHeight(int height)
			{
				m_Height = static_cast<Height>(height);
			}
	};
}

namespace KxDataView2
{
	class KX_API ProgressValue: public TextValue, public ProgressValueBase
	{
		public:
			ProgressValue() = default;
			ProgressValue(int position, int range, ProgressState state = ProgressState::Normal)
				:ProgressValueBase(position, range, state)
			{
			}
			ProgressValue(int position, int range, const wxString& text = wxEmptyString, ProgressState state = ProgressState::Normal)
				:ProgressValueBase(position, range, state), TextValue(text)
			{
			}
			ProgressValue(int position, const wxString& text = wxEmptyString, ProgressState state = ProgressState::Normal)
				:TextValue(text), ProgressValueBase(position, state)
			{
			}

		public:
			bool FromAny(const wxAny& value);
			void Clear()
			{
				TextValue::Clear();
				ProgressValueBase::Clear();
			}
	};
}

namespace KxDataView2
{
	class KX_API ProgressRenderer: public Renderer
	{
		private:
			ProgressValue m_Value;

		protected:
			bool SetValue(const wxAny& value) override;
			ToolTip CreateToolTip() const override
			{
				return ToolTip::CreateDefaultForRenderer(m_Value.GetText());
			}
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			ProgressRenderer(int alignment = wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL)
				:Renderer(alignment)
			{
			}
	};
}
