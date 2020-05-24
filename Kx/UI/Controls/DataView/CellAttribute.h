#pragma once
#include "Common.h"
#include "CellAttributeOptions.h"

namespace kxf::UI::DataView
{
	class KX_API CellAttribute final
	{
		private:
			CellAttributes::Options m_Options;
			CellAttributes::BGOptions m_BGOptions;
			CellAttributes::FontOptions m_FontOptions;

		public:
			bool IsDefault() const
			{
				return m_Options.IsDefault() && m_BGOptions.IsDefault() && m_FontOptions.IsDefault();
			}
			void Reset()
			{
				*this = CellAttribute();
			}
			wxFont GetEffectiveFont(const wxFont& baseFont) const;

		public:
			const CellAttributes::Options& Options() const
			{
				return m_Options;
			}
			CellAttributes::Options& Options()
			{
				return m_Options;
			}
			
			const CellAttributes::BGOptions& BGOptions() const
			{
				return m_BGOptions;
			}
			CellAttributes::BGOptions& BGOptions()
			{
				return m_BGOptions;
			}

			const CellAttributes::FontOptions& FontOptions() const
			{
				return m_FontOptions;
			}
			CellAttributes::FontOptions& FontOptions()
			{
				return m_FontOptions;
			}
	};
}
