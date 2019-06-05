#pragma once
#include "Common.h"
#include "CellAttributeOptions.h"

namespace KxDataView2
{
	using CellOption = CellAttributeOptions::Enums::Option;
	using CellBGOption = CellAttributeOptions::Enums::BGOption;
	using CellFontOption = CellAttributeOptions::Enums::FontOption;
}

namespace KxDataView2
{
	class KX_API CellAttributes
	{
		private:
			CellAttributeOptions::Options m_Options;
			CellAttributeOptions::BGOptions m_BGOptions;
			CellAttributeOptions::FontOptions m_FontOptions;

		public:
			bool IsDefault() const
			{
				return m_Options.IsDefault() && m_BGOptions.IsDefault() && m_BGOptions.IsDefault();
			}
			void Reset()
			{
				*this = CellAttributes();
			}
			wxFont GetEffectiveFont(const wxFont& baseFont) const;

		public:
			const CellAttributeOptions::Options& Options() const
			{
				return m_Options;
			}
			CellAttributeOptions::Options& Options()
			{
				return m_Options;
			}
			
			const CellAttributeOptions::BGOptions& BGOptions() const
			{
				return m_BGOptions;
			}
			CellAttributeOptions::BGOptions& BGOptions()
			{
				return m_BGOptions;
			}

			const CellAttributeOptions::FontOptions& FontOptions() const
			{
				return m_FontOptions;
			}
			CellAttributeOptions::FontOptions& FontOptions()
			{
				return m_FontOptions;
			}
	};
}
