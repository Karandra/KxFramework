#pragma once
#include "Common.h"
#include "FSPath.h"
#include "KxFramework/KxWinUndef.h"

namespace KxFramework
{
	class KX_API LegacyVolume final
	{
		public:
			static size_t EnumVolumes(std::function<bool(LegacyVolume)> func);

			static LegacyVolume FromChar(const wxUniChar& value)
			{
				LegacyVolume drive;
				drive.AssignFromChar(value);
				return drive;
			}
			static LegacyVolume FromChar(char value)
			{
				return FromChar(wxUniChar(value));
			}
			static LegacyVolume FromChar(wchar_t value)
			{
				return FromChar(wxUniChar(value));
			}
			static LegacyVolume FromChar(const wxString& value)
			{
				LegacyVolume drive;
				if (!value.IsEmpty())
				{
					drive.AssignFromChar(value[0]);
				}
				return drive;
			}
			static LegacyVolume FromIndex(int index)
			{
				LegacyVolume drive;
				drive.AssignFromIndex(index);
				return drive;
			}

		private:
			void AssignFromChar(const wxUniChar& value);
			void AssignFromIndex(int index);
			wxString DoGetPath() const;

		private:
			char m_Drive = 0;

		public:
			LegacyVolume() = default;
			LegacyVolume(const LegacyVolume&) = default;

		public:
			bool IsValid() const;
			bool DoesExist() const;

			FSPath GetPath() const;
			int GetIndex() const;
			wxUniChar GetChar() const;

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}
	};
}
