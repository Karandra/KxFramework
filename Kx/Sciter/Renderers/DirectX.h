#pragma once
#include "Kx/Sciter/IWindowRenderer.h"
#include "FPSCounter.h"
#include <KxFramework/KxCOM.h>

#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dcomp.h>

namespace KxSciter
{
	class KX_API DirectX: public IWindowRenderer
	{
		private:
			Host& m_SciterHost;
			wxWindow& m_SciterWindow;
			FPSCounter m_FrameCounter;

			D3D_DRIVER_TYPE m_DriverType = D3D_DRIVER_TYPE_NULL;
			D3D_FEATURE_LEVEL m_FeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;

			KxCOMPtr<IDXGIFactory2> m_DXGIFactory;
			KxCOMPtr<IDXGIDevice> m_DXGIDevice;
			KxCOMPtr<IDXGISwapChain1> m_SwapChain;
			KxCOMPtr<ID3D11Device1> m_D3DDevice;
			KxCOMPtr<ID3D11DeviceContext1> m_DeviceContext;

			KxCOMPtr<IDCompositionDevice> m_CompositionDevice;
			KxCOMPtr<IDCompositionTarget> m_CompositionTarget;
			KxCOMPtr<IDCompositionVisual> m_CompositionVisual;

		public:
			DirectX(Host& host);
			~DirectX();

		public:
			bool Create() override;
			void Render() override;
			void OnSize() override;
			void OnIdle() override;

			double GetFPS() const override
			{
				return m_FrameCounter.GetCount();
			}
	};
}
