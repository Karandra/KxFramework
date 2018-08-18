#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewEditor.h"

class KxDataViewSpinEditor: public KxDataViewEditor
{
	private:
		enum class Type
		{
			Integer,
			Float,
		};

	private:
		Type m_Type = Type::Integer;
		Type m_EffectiveType = Type::Integer;
		bool m_IsWrapping = false;

		int m_IntMin = std::numeric_limits<int>::lowest();
		int m_IntMax = std::numeric_limits<int>::max();
		int m_IntBase = 10;
		int m_IntIncrement = 1;

		double m_FloatMin = std::numeric_limits<double>::lowest();
		double m_FloatMax = std::numeric_limits<double>::max();
		double m_FloatIncrement = 1.0;
		size_t m_FloatPrecision = 1;

	private:
		bool ShouldMimicIntegerUsingFloat() const
		{
			return m_Type == Type::Integer && m_IntIncrement != 1 && m_IntBase == 10;
		}
		template<class T> T CastAndGetValue(wxWindow* control) const
		{
			if constexpr(std::is_floating_point<T>::value)
			{
				return static_cast<wxSpinCtrlDouble*>(control)->GetValue();
			}
			else
			{
				return static_cast<wxSpinCtrl*>(control)->GetValue();
			}
		}

	protected:
		virtual wxWindow* CreateEditorControl(wxWindow* parent, const wxRect& cellRect, const wxAny& value) override;
		virtual bool GetValueFromEditor(wxWindow* control, wxAny& value) const override;

	public:
		/* General */
		bool IsIntegerType() const
		{
			return m_Type == Type::Integer;
		}
		bool IsFloatType() const
		{
			return m_Type == Type::Float;
		}
		void SetIntergerType()
		{
			m_Type = Type::Integer;
		}
		void SetFloatType()
		{
			m_Type = Type::Float;
		}

		bool IsWrapping() const
		{
			return m_IsWrapping;
		}
		void SetWrap(bool wrap)
		{
			m_IsWrapping = wrap;
		}

		/* Integer */
		void SetRangeInt(int min, int max)
		{
			if (min > max)
			{
				std::swap(min, max);
			}

			m_IntMin = min;
			m_IntMax = max;
		}
		int GetMinInt() const
		{
			return m_IntMin;
		}
		int GetMaxInt() const
		{
			return m_IntMax;
		}
		
		int GetIncrementInt() const
		{
			return m_IntIncrement;
		}
		void SetIncrementInt(int value)
		{
			if (value < 1)
			{
				value = 1;
			}
			m_IntIncrement = value;
		}

		int GetBase() const
		{
			return m_IntBase;
		}
		void SetBase(int value)
		{
			if (value != 10 && value != 16)
			{
				value = 10;
			}
			m_IntBase = value;
		}

		/* Floating point */
		void SetRangeFloat(double min, double max)
		{
			if (min > max)
			{
				std::swap(min, max);
			}

			m_FloatMin = min;
			m_FloatMax = max;
		}
		double GetMinFloat() const
		{
			return m_FloatMin;
		}
		double GetMaxFloat() const
		{
			return m_FloatMax;
		}

		double GetIncrementFloat() const
		{
			return m_FloatIncrement;
		}
		void SetIncrementFloat(double value)
		{
			if (value < 1.0)
			{
				value = 1.0;
			}
			m_FloatIncrement = value;
		}

		size_t GetPrecision() const
		{
			return m_FloatPrecision;
		}
		void SetPrecision(size_t value)
		{
			m_FloatPrecision = value;
		}
};
