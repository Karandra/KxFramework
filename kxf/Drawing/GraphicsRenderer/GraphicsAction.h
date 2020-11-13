#pragma once
#include "Common.h"
#include "IGraphicsContext.h"

namespace kxf::GraphicsAction
{
	class ChangeAntialiasMode final
	{
		private:
			IGraphicsContext& m_GC;
			const AntialiasMode m_OriginalMode = AntialiasMode::None;

		public:
			ChangeAntialiasMode(IGraphicsContext& gc, AntialiasMode newMode)
				:m_GC(gc), m_OriginalMode(gc.GetAntialiasMode())
			{
				m_GC.SetAntialiasMode(newMode);
			}
			~ChangeAntialiasMode()
			{
				m_GC.SetAntialiasMode(m_OriginalMode);
			}
	};

	class ChangeCompositionMode final
	{
		private:
			IGraphicsContext& m_GC;
			const CompositionMode m_OriginalMode = CompositionMode::None;

		public:
			ChangeCompositionMode(IGraphicsContext& gc, CompositionMode newMode)
				:m_GC(gc), m_OriginalMode(gc.GetCompositionMode())
			{
				m_GC.SetCompositionMode(newMode);
			}
			~ChangeCompositionMode()
			{
				m_GC.SetCompositionMode(m_OriginalMode);
			}
	};

	class ChangeInterpolationQuality final
	{
		private:
			IGraphicsContext& m_GC;
			const InterpolationQuality m_OriginalQuality = InterpolationQuality::None;

		public:
			ChangeInterpolationQuality(IGraphicsContext& gc, InterpolationQuality newQuality)
				:m_GC(gc), m_OriginalQuality(gc.GetInterpolationQuality())
			{
				m_GC.SetInterpolationQuality(newQuality);
			}
			~ChangeInterpolationQuality()
			{
				m_GC.SetInterpolationQuality(m_OriginalQuality);
			}
	};

	class BeginLayer final
	{
		private:
			IGraphicsContext& m_GC;

		public:
			BeginLayer(IGraphicsContext& gc, float opacity)
				:m_GC(gc)
			{
				m_GC.BeginLayer(opacity);
			}
			~BeginLayer()
			{
				m_GC.EndLayer();
			}
	};

	class PushState final
	{
		private:
			IGraphicsContext& m_GC;

		public:
			PushState(IGraphicsContext& gc)
				:m_GC(gc)
			{
				m_GC.PushState();
			}
			~PushState()
			{
				m_GC.PopState();
			}
	};

	class StartDocument final
	{
		private:
			IGraphicsContext& m_GC;
			const bool m_Result = false;

		public:
			StartDocument(IGraphicsContext& gc, const String& message)
				:m_GC(gc), m_Result(gc.StartDocument(message))
			{
			}
			~StartDocument()
			{
				m_GC.EndDocument();
			}

		public:
			operator bool() const
			{
				return m_Result;
			}
	};

	class Clip final
	{
		private:
			IGraphicsContext& m_GC;

		public:
			Clip(IGraphicsContext& gc, const Rect& rect)
				:m_GC(gc)
			{
				m_GC.PushState();
				m_GC.SetClippingRegion(rect);
			}
			Clip(IGraphicsContext& gc, const Region& region)
				:m_GC(gc)
			{
				m_GC.PushState();
				m_GC.SetClippingRegion(region);
			}
			~Clip()
			{
				m_GC.PopState();
			}
	};
}
