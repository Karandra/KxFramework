#pragma once
#include "KxFramework/KxFramework.h"

class KxColor;
class KxSystemSettings
{
	public:
		struct DisplayInfo
		{
			int Width = 0;
			int Height = 0;
			int Depth = 0;
			int Frequency = 0;
		};
		struct VideoMode
		{
			DWORD Width = 0;
			DWORD Height = 0;
			DWORD Frequency = 0;
			DWORD Depth = 0;
		};
		typedef std::vector<VideoMode> VideoModeList;
		typedef std::vector<DISPLAY_DEVICE> VideoAdapterList;

	public:
		static KxColor GetColor(wxSystemColour index);
		static int GetMetric(wxSystemMetric index, const wxWindow* window = NULL);
		static KxStringVector GetSoundsList();
		static DisplayInfo GetDisplayInfo();
		static VideoAdapterList EnumVideoAdapters();
		static VideoModeList EnumVideoModes(const wxString& deviceName);
};
