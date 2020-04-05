#pragma once
#include "KxFramework/KxFramework.h"

#include "Kx/General/Color.h"
using KxColor = KxFramework::Color;

class KX_API KxSystemSettings
{
	public:
		struct KX_API DisplayInfo
		{
			int Width = 0;
			int Height = 0;
			int Depth = 0;
			int Frequency = 0;
		};
		struct KX_API VideoMode
		{
			DWORD Width = 0;
			DWORD Height = 0;
			DWORD Frequency = 0;
			DWORD Depth = 0;
		};
		using VideoModeList = std::vector<VideoMode>;
		using VideoAdapterList = std::vector<DISPLAY_DEVICE>;

	public:
		static KxColor GetColor(wxSystemColour index);
		static int GetMetric(wxSystemMetric index, const wxWindow* window = nullptr);
		static KxStringVector GetSoundsList();
		static DisplayInfo GetDisplayInfo();
		static VideoAdapterList EnumVideoAdapters();
		static VideoModeList EnumVideoModes(const wxString& deviceName);
};
