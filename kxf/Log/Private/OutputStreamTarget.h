#pragma once
#include "../Common.h"
#include "kxf/General/OptionalPtr.h"
#include "kxf/IO/IStream.h"
#include <wx/log.h>

namespace kxf::Log
{
	class KX_API OutputStreamTarget final: public wxLog
	{
		private:
			optional_ptr<IOutputStream> m_Stream;

		protected:
			void DoLogText(const wxString& logMessage) override;

		public:
			OutputStreamTarget(IOutputStream& stream) noexcept
				:m_Stream(stream)
			{
			}
			OutputStreamTarget(std::unique_ptr<IOutputStream> stream) noexcept
				:m_Stream(std::move(stream))
			{
			}

		public:
			void Flush() override;

			std::unique_ptr<wxLogFormatter> SetFormatter(std::unique_ptr<wxLogFormatter> logFormatter)
			{
				return std::unique_ptr<wxLogFormatter>(wxLog::SetFormatter(logFormatter.release()));
			}
	};
}
