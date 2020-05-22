#pragma once
#include "../Renderer.h"

namespace KxFramework::UI::DataView
{
	class KX_API ToggleValue
	{
		private:
			ToggleState m_State = ToggleState::None;
			ToggleType m_Type = ToggleType::None;
			bool m_Allow3State = false;

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
			bool FromAny(const wxAny& value);
			void Clear()
			{
				*this = {};
			}

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
			void ClearState()
			{
				m_State = ToggleState::None;
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
			void ClearType()
			{
				m_Type = ToggleType::None;
			}
	
			bool Is3StateAllowed() const
			{
				return m_Allow3State;
			}
			void Allow3State(bool allow = true)
			{
				m_Allow3State = allow;
			}
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API ToggleRendererBase
	{
		private:
			ToggleValue& m_Value;

		protected:
			bool DoOnActivateCell(const Rect& toggleRect, ToggleState& state, const wxMouseEvent* mouseEvent = nullptr) const;

		public:
			ToggleRendererBase(ToggleValue& value)
				:m_Value(value)
			{
			}
	};
}

namespace KxFramework::UI::DataView
{
	class KX_API ToggleRenderer: public Renderer, public ToggleRendererBase
	{
		private:
			ToggleValue m_Value;

		protected:
			bool HasActivator() const override
			{
				return true;
			}
			wxAny OnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent = nullptr) override;
			
			bool SetValue(const wxAny& value) override;
			void DrawCellContent(const Rect& cellRect, CellState cellState) override;
			Size GetCellSize() const override;

		public:
			ToggleRenderer(int alignment = wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL)
				:Renderer(alignment), ToggleRendererBase(m_Value)
			{
			}
			
		public:
			wxString GetTextValue(const wxAny& value) const override
			{
				return {};
			}
	};
}
