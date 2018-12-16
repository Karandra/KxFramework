#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/graphics.h>
#include <wx/geometry.h>

class KX_API KxGraphicsContext
{
	private:
		wxGraphicsContext* m_Context = NULL;

	private:
		template<class ReturnType, class FuncT = ReturnType(wxGraphicsContext::*)(wxDouble x, wxDouble y, wxDouble w, wxDouble h)>
		class CallMember
		{
			private:
				wxGraphicsContext* m_Context = NULL;
				FuncT m_Func = NULL;

			public:
				CallMember(wxGraphicsContext* context, const FuncT& func)
					:m_Context(context), m_Func(func)
				{
				}

			public:
				template<class... Args> ReturnType Call(Args... args)
				{
					return (m_Context->*m_Func)(std::forward<Args>(args)...);
				}

				template<class RectT, class... Args> ReturnType CallWithRect(const RectT& rect, Args... args)
				{
					if constexpr(std::is_base_of<wxRect, RectT>::value)
					{
						return Call(std::forward<Args>(args)..., rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
					}
					else if constexpr(std::is_base_of<wxRect2DDouble, RectT>::value)
					{
						return Call(std::forward<Args>(args)..., rect.m_x, rect.m_y, rect.m_width, rect.m_height);
					}
					return ReturnType();
				}
		};

	public:
		KxGraphicsContext(wxGraphicsContext* context)
			:m_Context(context)
		{
		}
		virtual ~KxGraphicsContext()
		{
		}

	public:
		wxRect2DDouble GetClip() const;
		wxRect GetClipInt() const;
		wxRegion GetClipRegion() const;

		void Clip(const wxRegion& region)
		{
			m_Context->Clip(region);
		}
		void Clip(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
		{
			m_Context->Clip(x, y, w, h);
		}
		void Clip(const wxRect& rect)
		{
			CallMember<void>(m_Context, &wxGraphicsContext::Clip).CallWithRect(rect);
		}
		void Clip(const wxRect2DDouble& rect)
		{
			CallMember<void>(m_Context, &wxGraphicsContext::Clip).CallWithRect(rect);
		}
		void ResetClip()
		{
			m_Context->ResetClip();
		}

		void DrawRectnagle(wxDouble x, wxDouble y, wxDouble w, wxDouble h)
		{
			m_Context->DrawRectangle(x, y, w, h);
		}
		void DrawRectnagle(const wxRect& rect)
		{
			CallMember<void>(m_Context, &wxGraphicsContext::DrawRectangle).CallWithRect(rect);
		}
		void DrawRectnagle(const wxRect2DDouble& rect)
		{
			CallMember<void>(m_Context, &wxGraphicsContext::DrawRectangle).CallWithRect(rect);
		}

		void DrawBitmap(const wxBitmap& bitmap, wxDouble x, wxDouble y, wxDouble w, wxDouble h)
		{
			m_Context->DrawBitmap(bitmap, x, y, w, h);
		}
		void DrawBitmap(const wxBitmap& bitmap, wxDouble x, wxDouble y)
		{
			m_Context->DrawBitmap(bitmap, x, y, bitmap.GetWidth(), bitmap.GetHeight());
		}
		void DrawBitmap(const wxBitmap& bitmap, const wxRect& rect)
		{
			CallMember<void, void(wxGraphicsContext::*)(const wxBitmap&, wxDouble, wxDouble, wxDouble, wxDouble)>
				(m_Context, &wxGraphicsContext::DrawBitmap).CallWithRect(rect, bitmap);
		}
		void DrawBitmap(const wxBitmap& bitmap, const wxRect2DDouble& rect)
		{
			CallMember<void, void(wxGraphicsContext::*)(const wxBitmap&, wxDouble, wxDouble, wxDouble, wxDouble)>
				(m_Context, &wxGraphicsContext::DrawBitmap).CallWithRect(rect, bitmap);
		}
};
