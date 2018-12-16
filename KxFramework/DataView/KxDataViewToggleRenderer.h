#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

class KX_API KxDataViewToggleValue
{
	public:
		enum ToggleState
		{
			InvalidState = -2,

			Checked = 1,
			Unchecked = 0,
			Indeterminate = -1,
		};
		enum ToggleType
		{
			InvalidType = -1,

			CheckBox,
			RadioBox,
		};

	private:
		ToggleState m_State = InvalidState;
		ToggleType m_Type = InvalidType;

	public:
		KxDataViewToggleValue(ToggleState state = InvalidState, ToggleType type = InvalidType)
			:m_State(state), m_Type(type)
		{
		}
		KxDataViewToggleValue(bool checked, ToggleType type = InvalidType)
			:m_Type(type)
		{
			SetChecked(checked);
		}

	public:
		bool HasState() const
		{
			return m_State != InvalidState;
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
			return m_Type != InvalidType;
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

class KX_API KxDataViewToggleRendererBase
{
	public:
		using ToggleState = KxDataViewToggleValue::ToggleState;
		using ToggleType = KxDataViewToggleValue::ToggleType;

	private:
		bool m_Allow3State = false;

		ToggleType m_DefaultType = ToggleType::CheckBox;
		ToggleState m_DefaultState = ToggleState::Unchecked;

	public:
		bool Is3StateAllowed() const
		{
			return m_Allow3State;
		}
		void SetAllow3State(bool allow)
		{
			m_Allow3State = allow;
		}

		ToggleType GetDefaultToggleType() const
		{
			return m_DefaultType;
		}
		void SetDefaultToggleType(ToggleType type)
		{
			m_DefaultType = type != ToggleType::InvalidType ? type : ToggleType::CheckBox;
		}

		ToggleState GetDefaultToggleState() const
		{
			return m_DefaultState;
		}
		void SetDefaultToggleState(ToggleState state)
		{
			m_DefaultState = state != ToggleState::InvalidState ? state : ToggleState::Unchecked;
		}
};

class KX_API KxDataViewToggleRenderer: public KxDataViewRenderer, public KxDataViewToggleRendererBase
{
	public:
		static bool GetValueAsToggleState(const wxAny& value, ToggleState& state);
		static wxSize GetToggleCellSizeNoMargins(wxWindow* window);
		static wxSize GetToggleCellSize(wxWindow* window);
		static bool DoActivateToggle(KxDataViewToggleRendererBase* rederer, wxWindow* window, const KxDataViewItem& item, const wxRect& cellRect, ToggleState& state, const wxMouseEvent* mouseEvent = NULL);
		static int DrawToggle(wxDC& dc, wxWindow* window, const wxRect& cellRect, KxDataViewCellState cellState, const KxDataViewToggleValue& value, bool cellEnabled);

	private:
		KxDataViewToggleValue m_Value;

	protected:
		virtual bool HasActivator() const override
		{
			return true;
		}
		virtual bool OnActivateCell(const KxDataViewItem& item, const wxRect& cellRect, const wxMouseEvent* mouseEvent = NULL);
		
		virtual bool IsEnabled() const override
		{
			return GetCellMode() & KxDATAVIEW_CELL_ACTIVATABLE && KxDataViewRenderer::IsEnabled();
		}
		virtual bool SetValue(const wxAny& value) override;

		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewToggleRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}
};
