#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"

namespace Kx::DataView2
{
	class KX_API ProgressValue
	{
		private:
			wxString m_Text;
			int m_Position = -1;
			ProgressState m_State = ProgressState::Normal;

		public:
			ProgressValue(int position = -1, const wxString& text = wxEmptyString, ProgressState state = ProgressState::Normal)
				:m_Text(text), m_State(state)
			{
				SetPosition(position);
			}

		public:
			bool HasText() const
			{
				return !m_Text.IsEmpty();
			}
			const wxString& GetText() const
			{
				return m_Text;
			}
			void SetText(const wxString& text)
			{
				m_Text = text;
			}

			int GetRange() const
			{
				return 100;
			}

			bool HasPosition() const
			{
				return m_Position >= 0;
			}
			int GetPosition() const
			{
				return m_Position;
			}
			void SetPosition(int position)
			{
				// Allow -1
				m_Position = std::clamp(position, -1, GetRange());
			}

			ProgressState GetState() const
			{
				return m_State;
			}
			void SetState(ProgressState state)
			{
				m_State = state;
			}
	};
}
namespace Kx::DataView2
{
	enum class ProgressHeight: int
	{
		Auto = -1,
		Fit = -2,
	};
}

namespace Kx::DataView2
{
	class KX_API ProgressRenderer: public Renderer
	{
		private:
			ProgressValue m_Value;
			ProgressHeight m_Height = ProgressHeight::Auto;

		protected:
			bool SetValue(const wxAny& value) override;
			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			ProgressRenderer(int alignment = wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL)
				:Renderer(alignment)
			{
			}

		public:
			ProgressHeight GetHeightOption() const
			{
				return m_Height;
			}
			int GetHeight() const
			{
				return static_cast<int>(m_Height);
			}
			void SetHeight(ProgressHeight height)
			{
				m_Height = height;
			}
			void SetHeight(int height)
			{
				m_Height = static_cast<ProgressHeight>(height);
			}
	};
}
