#pragma once
#include "KxFramework/KxFramework.h"

class KxGCAntialiasModeChanger final
{
	private:
		wxGraphicsContext& m_GC;
		const wxAntialiasMode m_OriginalMode = wxANTIALIAS_NONE;

	public:
		KxGCAntialiasModeChanger(wxGraphicsContext& gc, wxAntialiasMode newMode)
			:m_GC(gc), m_OriginalMode(gc.GetAntialiasMode())
		{
			m_GC.SetAntialiasMode(newMode);
		}
		~KxGCAntialiasModeChanger()
		{
			m_GC.SetAntialiasMode(m_OriginalMode);
		}
};

class KxGCCompositionModeChanger final
{
	private:
		wxGraphicsContext& m_GC;
		const wxCompositionMode m_OriginalMode = wxCOMPOSITION_INVALID;

	public:
		KxGCCompositionModeChanger(wxGraphicsContext& gc, wxCompositionMode newMode)
			:m_GC(gc), m_OriginalMode(gc.GetCompositionMode())
		{
			m_GC.SetCompositionMode(newMode);
		}
		~KxGCCompositionModeChanger()
		{
			m_GC.SetCompositionMode(m_OriginalMode);
		}
};

class KxGCInterpolationQualityChanger final
{
	private:
		wxGraphicsContext& m_GC;
		const wxInterpolationQuality m_OriginalQuality = wxINTERPOLATION_NONE;

	public:
		KxGCInterpolationQualityChanger(wxGraphicsContext& gc, wxInterpolationQuality newQuality)
			:m_GC(gc), m_OriginalQuality(gc.GetInterpolationQuality())
		{
			m_GC.SetInterpolationQuality(newQuality);
		}
		~KxGCInterpolationQualityChanger()
		{
			m_GC.SetInterpolationQuality(m_OriginalQuality);
		}
};


class KxGCBeginLayer final
{
	private:
		wxGraphicsContext& m_GC;

	public:
		KxGCBeginLayer(wxGraphicsContext& gc, wxDouble opacity)
			:m_GC(gc)
		{
			m_GC.BeginLayer(opacity);
		}
		~KxGCBeginLayer()
		{
			m_GC.EndLayer();
		}
};

class KxGCPushState final
{
	private:
		wxGraphicsContext& m_GC;

	public:
		KxGCPushState(wxGraphicsContext& gc)
			:m_GC(gc)
		{
			m_GC.PushState();
		}
		~KxGCPushState()
		{
			m_GC.PopState();
		}
};

class KxGCStartDocument final
{
	private:
		wxGraphicsContext& m_GC;
		const bool m_Result = false;

	public:
		KxGCStartDocument(wxGraphicsContext& gc, const wxString& message)
			:m_GC(gc), m_Result(gc.StartDoc(message))
		{
		}
		~KxGCStartDocument()
		{
			m_GC.EndDoc();
		}

	public:
		operator bool() const
		{
			return m_Result;
		}
};

class KxGCClipper final
{
	private:
		wxGraphicsContext& m_GC;

	public:
		KxGCClipper(wxGraphicsContext& gc, const wxRect& rect)
			:m_GC(gc)
		{
			m_GC.PushState();
			m_GC.Clip(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
		}
		KxGCClipper(wxGraphicsContext& gc, const wxRegion& region)
			:m_GC(gc)
		{
			m_GC.PushState();
			m_GC.Clip(region);
		}
		~KxGCClipper()
		{
			m_GC.PopState();
		}
};