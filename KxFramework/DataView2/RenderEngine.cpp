#include "KxStdAfx.h"
#include "RenderEngine.h"
#include "Renderer.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "KxFramework/KxUxTheme.h"
#include "wx/generic/private/markuptext.h"

namespace
{
	bool operator>(const wxSize& v1, const wxSize& v2)
	{
		return v1.GetWidth() > v2.GetWidth() || v1.GetHeight() > v2.GetHeight();
	}
}

namespace Kx::DataView2::Markup
{
	using MarkupMode = Renderer::MarkupMode;
	template<MarkupMode t_Mode> auto Create(const wxString& string = {})
	{
		if constexpr(t_Mode == MarkupMode::TextOnly)
		{
			return wxItemMarkupText(string);
		}
		else if constexpr(t_Mode == MarkupMode::WithMnemonics)
		{
			return wxMarkupText(string);
		}
	}

	wxSize GetTextExtent(const wxMarkupTextBase& markup, wxDC& dc)
	{
		return markup.Measure(dc);
	}
	wxSize GetTextExtent(MarkupMode mode, wxDC& dc, const wxString& string)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				return GetTextExtent(Create<MarkupMode::TextOnly>(string), dc);
			}
			case MarkupMode::WithMnemonics:
			{
				return GetTextExtent(Create<MarkupMode::WithMnemonics>(string), dc);
			}
		};
		return wxSize(0, 0);
	}
	
	template<class T> void DrawText(T& markup, wxWindow* window, wxDC& dc, const wxRect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		if constexpr(std::is_same_v<T, wxMarkupText>)
		{
			markup.Render(dc, rect, flags);
		}
		else
		{
			markup.Render(window, dc, rect, flags, ellipsizeMode);
		}
	}
	void DrawText(MarkupMode mode, const wxString& string, wxWindow* window, wxDC& dc, const wxRect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				auto markup = Create<MarkupMode::TextOnly>(string);
				return DrawText(markup, window, dc, rect, flags, ellipsizeMode);
			}
			case MarkupMode::WithMnemonics:
			{
				auto markup = Create<MarkupMode::WithMnemonics>(string);
				return DrawText(markup, window, dc, rect, flags, ellipsizeMode);
			}
		};
	}
}

namespace Kx::DataView2
{
	int RenderEngine::CalcCenter(int cellSize, int itemSize) const
	{
		const int margins = cellSize - itemSize;

		// Can't offset by fractional values, so return 1.
		if (margins > 0 && margins <= 3)
		{
			return 1;
		}
		return margins / 2;
	}
	int RenderEngine::FindFirstNewLinePos(const wxString& string) const
	{
		int pos = string.Find('\r');
		if (pos == wxNOT_FOUND)
		{
			pos = string.Find('\n');
		}
		return pos;
	}
	int RenderEngine::GetControlFlags(CellState cellState) const
	{
		int flags = wxCONTROL_NONE;

		if (!m_Renderer.GetAttributes().IsEnabled())
		{
			flags |= wxCONTROL_DISABLED;
		}

		if (cellState.IsSelected())
		{
			flags |= wxCONTROL_PRESSED;
		}

		if (cellState.IsHotTracked())
		{
			flags |= wxCONTROL_CURRENT|wxCONTROL_FOCUSED;
		}
		
		return flags;
	}

	wxSize RenderEngine::GetTextExtent(const wxString& string) const
	{
		// Regular (GDI) device context is preferable to draw and measure text
		if (m_Renderer.HasRegularDC())
		{
			return GetTextExtent(m_Renderer.GetRegularDC(), string);
		}
		else if (m_Renderer.HasGraphicsDC())
		{
			return GetTextExtent(m_Renderer.GetGraphicsDC(), string);
		}
		else
		{
			// No existing window context right now, create one to measure text
			wxClientDC dc(m_Renderer.GetView());
			return GetTextExtent(dc, string);
		}
	}
	wxSize RenderEngine::GetTextExtent(wxDC& dc, const wxString& string) const
	{
		wxSize textExtent(0, 0);
		if (m_Renderer.m_Attributes.HasFontAttributes())
		{
			wxFont font(m_Renderer.m_Attributes.GetEffectiveFont(dc.GetFont()));
			int newLinePos = FindFirstNewLinePos(string);

			if (newLinePos != wxNOT_FOUND)
			{
				if (m_Renderer.IsMarkupEnabled())
				{
					textExtent = Markup::GetTextExtent(m_Renderer.m_MarkupMode, dc, string);
				}
				else
				{
					dc.GetTextExtent(string.Left(newLinePos), &textExtent.x, &textExtent.y, nullptr, nullptr, &font);
				}
			}
			else
			{
				if (m_Renderer.IsMarkupEnabled())
				{
					textExtent = Markup::GetTextExtent(m_Renderer.m_MarkupMode, dc, string);
				}
				else
				{
					dc.GetTextExtent(string, &textExtent.x, &textExtent.y, nullptr, nullptr, &font);
				}
			}
			
		}
		else
		{
			textExtent = dc.GetTextExtent(string);
		}
		return textExtent;
	}

	bool RenderEngine::DrawText(const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX)
	{
		if (m_Renderer.HasRegularDC())
		{
			return DrawText(m_Renderer.GetRegularDC(), cellRect, cellState, string, offsetX);
		}
		else if (m_Renderer.HasGraphicsDC())
		{
			return DrawText(m_Renderer.GetGraphicsDC(), cellRect, cellState, string, offsetX);
		}
		return false;
	}
	bool RenderEngine::DrawText(wxDC& dc, const wxRect& cellRect, CellState cellState, const wxString& string, int offsetX)
	{
		if (!string.IsEmpty())
		{
			wxRect textRect = cellRect;
			textRect.x += offsetX;
			textRect.width -= offsetX;

			int flags = 0;
			if (m_Renderer.IsMarkupWithMnemonicsEnabled() && m_Renderer.m_Attributes.ShouldShowAccelerators())
			{
				flags |= wxMarkupText::Render_ShowAccels;
			}

			int newLinePos = FindFirstNewLinePos(string);
			if (newLinePos != wxNOT_FOUND)
			{
				if (m_Renderer.IsMarkupEnabled())
				{
					Markup::DrawText(m_Renderer.m_MarkupMode, string, m_Renderer.GetView(), dc, textRect, flags, m_Renderer.GetEllipsizeMode());
				}
				else
				{
					dc.DrawText(wxControl::Ellipsize(string, dc, m_Renderer.GetEllipsizeMode(), textRect.GetWidth()), textRect.GetPosition());
				}
			}
			else
			{
				if (m_Renderer.IsMarkupEnabled())
				{
					Markup::DrawText(m_Renderer.m_MarkupMode, string, m_Renderer.GetView(), dc, textRect, flags, m_Renderer.GetEllipsizeMode());
				}
				else
				{
					dc.DrawText(wxControl::Ellipsize(string, dc, m_Renderer.GetEllipsizeMode(), textRect.GetWidth()), textRect.GetPosition());
				}
			}
			return true;
		}
		return false;
	}

	bool RenderEngine::DrawBitmap(const wxRect& cellRect, CellState cellState, const wxBitmap& bitmap)
	{
		if (bitmap.IsOk())
		{
			const wxPoint pos = cellRect.GetPosition();
			const wxSize size = bitmap.GetSize();

			m_Renderer.GetGraphicsContext().DrawBitmap(m_Renderer.m_Attributes.IsEnabled() ? bitmap : bitmap.ConvertToDisabled(), pos.x, pos.y, size.GetWidth(), size.GetHeight());
			return true;
		}
		return false;
	}
	bool RenderEngine::DrawProgressBar(const wxRect& cellRect, CellState cellState, int value, int range, ProgressBarState state)
	{
		KxUxTheme::Handle themeHandle(m_Renderer.GetView(), L"PROGRESS");
		if (themeHandle)
		{
			HDC dc = m_Renderer.GetGraphicsDC().GetHDC();

			// Draw background
			RECT cellRectWin = KxUtility::CopyRectToRECT(cellRect);
			::DrawThemeBackground(themeHandle, dc, PP_BAR, 0, &cellRectWin, nullptr);

			// Draw filled part
			RECT contentRect = {0};
			::GetThemeBackgroundContentRect(themeHandle, dc, PP_BAR, 0, &cellRectWin, &contentRect);

			contentRect.right = contentRect.left + wxMulDivInt32(contentRect.right - contentRect.left, value, range);
			if (contentRect.left - 2 == cellRectWin.left)
			{
				contentRect.left++;
			}
			if (contentRect.right + 2 == cellRectWin.right)
			{
				contentRect.right--;
			}

			switch (state)
			{
				case ProgressBarState::Paused:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_PAUSED, &contentRect, nullptr);
					break;
				}
				case ProgressBarState::Error:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_ERROR, &contentRect, nullptr);
					break;
				}
				case ProgressBarState::Partial:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_PARTIAL, &contentRect, nullptr);
					break;
				}

				default:
				case ProgressBarState::Normal:
				{
					::DrawThemeBackground(themeHandle, dc, PP_FILL, PBFS_NORMAL, &contentRect, nullptr);
					break;
				}
			};
			return true;
		}

		wxRendererNative::Get().DrawGauge(m_Renderer.GetView(), m_Renderer.GetGraphicsDC(), cellRect, value, range);
		return true;
	}
	
	wxSize RenderEngine::GetToggleSize() const
	{
		return wxRendererNative::Get().GetCheckBoxSize(m_Renderer.GetView());
	}
	wxSize RenderEngine::DrawToggle(wxDC& dc, const wxRect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType)
	{
		int flags = GetControlFlags(cellState);
		switch (toggleState)
		{
			case ToggleState::Checked:
			{
				flags |= wxCONTROL_CHECKED;
				break;
			}
			case ToggleState::Indeterminate:
			{
				flags |= wxCONTROL_UNDETERMINED;
				break;
			}
		};

		// Ensure that the check boxes always have at least the minimal required size,
		// otherwise DrawCheckBox() doesn't really work well. If this size is greater than
		// the cell size, the checkbox will be truncated but this is a lesser evil.
		View* view = m_Renderer.GetView();
		wxRect toggleRect = cellRect;
		wxSize size = toggleRect.GetSize();
		size.IncTo(GetToggleSize());
		toggleRect.SetSize(size);

		if (toggleType == ToggleType::CheckBox || flags & wxCONTROL_UNDETERMINED)
		{
			wxRendererNative::Get().DrawCheckBox(view, dc, toggleRect, flags);
		}
		else
		{
			wxRendererNative::Get().DrawRadioBitmap(view, dc, toggleRect, flags);
		}
		return toggleRect.GetSize();
	}
}
