#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"

namespace Kx::DataView2
{
	class KX_API ToggleValue
	{
		private:
			ToggleState m_State = ToggleState::None;
			ToggleType m_Type = ToggleType::None;

		public:
			ToggleValue(ToggleState state = ToggleState::None, ToggleType type = ToggleType::None)
				:m_State(state), m_Type(type)
			{
			}
			ToggleValue(bool checked, ToggleType type = ToggleType::None)
				:m_Type(type)
			{
				SetChecked(checked);
			}

		public:
			bool HasState() const
			{
				return m_State != ToggleState::None;
			}
			ToggleState& GetState()
			{
				return m_State;
			}
			ToggleState GetState() const
			{
				return m_State;
			}
			void SetState(ToggleState state)
			{
				m_State = state;
			}
			void SetChecked(bool checked)
			{
				m_State = checked ? ToggleState::Checked : ToggleState::Unchecked;
			}

			bool HasType() const
			{
				return m_Type != ToggleType::None;
			}
			ToggleType GetType() const
			{
				return m_Type;
			}
			void SetType(ToggleType type)
			{
				m_Type = type;
			}
	};
}

namespace Kx::DataView2
{
	class KX_API ToggleRendererBase
	{
		private:
			ToggleType m_DefaultType = ToggleType::CheckBox;
			ToggleState m_DefaultState = ToggleState::Unchecked;
			bool m_Allow3State = false;

		protected:
			bool DoOnActivateCell(const wxRect& toggleRect, ToggleState& state, const wxMouseEvent* mouseEvent = nullptr) const;

		public:
			bool Is3StateAllowed() const
			{
				return m_Allow3State;
			}
			void Allow3State(bool allow = true)
			{
				m_Allow3State = allow;
			}

			ToggleType GetDefaultToggleType() const
			{
				return m_DefaultType;
			}
			void SetDefaultToggleType(ToggleType type)
			{
				m_DefaultType = type != ToggleType::None ? type : ToggleType::CheckBox;
			}

			ToggleState GetDefaultToggleState() const
			{
				return m_DefaultState;
			}
			void SetDefaultToggleState(ToggleState state)
			{
				m_DefaultState = state != ToggleState::None ? state : ToggleState::Unchecked;
			}
	};
}

namespace Kx::DataView2
{
	class KX_API ToggleRenderer: public Renderer, public ToggleRendererBase
	{
		public:
			static bool GetValueAsToggleState(const wxAny& value, ToggleState& state);
			static ToggleState GetValueAsToggleState(const wxAny& value)
			{
				ToggleState state = ToggleState::None;
				GetValueAsToggleState(value, state);
				return state;
			}

		private:
			ToggleValue m_Value;

		protected:
			bool HasActvator() const override
			{
				return true;
			}
			wxAny OnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent = nullptr) override;
			
			bool SetValue(const wxAny& value) override;
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			ToggleRenderer(int alignment = wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL)
				:Renderer(alignment)
			{
			}
	};
}
