#pragma once
#include "Common.h"
#include <wx/graphics.h>

namespace KxFramework
{
	class GCAChangentialiasMode final
	{
		private:
			wxGraphicsContext& m_GC;
			const wxAntialiasMode m_OriginalMode = wxAntialiasMode::wxANTIALIAS_NONE;

		public:
			GCAChangentialiasMode(wxGraphicsContext& gc, wxAntialiasMode newMode)
				:m_GC(gc), m_OriginalMode(gc.GetAntialiasMode())
			{
				m_GC.SetAntialiasMode(newMode);
			}
			~GCAChangentialiasMode()
			{
				m_GC.SetAntialiasMode(m_OriginalMode);
			}
	};

	class GCChangeCompositionMode final
	{
		private:
			wxGraphicsContext& m_GC;
			const wxCompositionMode m_OriginalMode = wxCompositionMode::wxCOMPOSITION_INVALID;

		public:
			GCChangeCompositionMode(wxGraphicsContext& gc, wxCompositionMode newMode)
				:m_GC(gc), m_OriginalMode(gc.GetCompositionMode())
			{
				m_GC.SetCompositionMode(newMode);
			}
			~GCChangeCompositionMode()
			{
				m_GC.SetCompositionMode(m_OriginalMode);
			}
	};

	class GCChangeInterpolationQuality final
	{
		private:
			wxGraphicsContext& m_GC;
			const wxInterpolationQuality m_OriginalQuality = wxInterpolationQuality::wxINTERPOLATION_NONE;

		public:
			GCChangeInterpolationQuality(wxGraphicsContext& gc, wxInterpolationQuality newQuality)
				:m_GC(gc), m_OriginalQuality(gc.GetInterpolationQuality())
			{
				m_GC.SetInterpolationQuality(newQuality);
			}
			~GCChangeInterpolationQuality()
			{
				m_GC.SetInterpolationQuality(m_OriginalQuality);
			}
	};

	class GCBeginLayer final
	{
		private:
			wxGraphicsContext& m_GC;

		public:
			GCBeginLayer(wxGraphicsContext& gc, wxDouble opacity)
				:m_GC(gc)
			{
				m_GC.BeginLayer(opacity);
			}
			~GCBeginLayer()
			{
				m_GC.EndLayer();
			}
	};

	class GCPushState final
	{
		private:
			wxGraphicsContext& m_GC;

		public:
			GCPushState(wxGraphicsContext& gc)
				:m_GC(gc)
			{
				m_GC.PushState();
			}
			~GCPushState()
			{
				m_GC.PopState();
			}
	};

	class GCStartDocument final
	{
		private:
			wxGraphicsContext& m_GC;
			const bool m_Result = false;

		public:
			GCStartDocument(wxGraphicsContext& gc, const wxString& message)
				:m_GC(gc), m_Result(gc.StartDoc(message))
			{
			}
			~GCStartDocument()
			{
				m_GC.EndDoc();
			}

		public:
			operator bool() const
			{
				return m_Result;
			}
	};

	class GCClip final
	{
		private:
			wxGraphicsContext& m_GC;

		public:
			GCClip(wxGraphicsContext& gc, const Rect& rect)
				:m_GC(gc)
			{
				m_GC.PushState();
				m_GC.Clip(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
			}
			GCClip(wxGraphicsContext& gc, const wxRegion& region)
				:m_GC(gc)
			{
				m_GC.PushState();
				m_GC.Clip(region);
			}
			~GCClip()
			{
				m_GC.PopState();
			}
	};
}
