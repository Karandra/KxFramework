#include "stdafx.h"
#include "RenderEngine.h"
#include "Renderer.h"
#include "Column.h"
#include "View.h"
#include "MainWindow.h"
#include "kxf/Drawing/UxTheme.h"
#include "kxf/Drawing/Private/UxThemeDefines.h"
#include "kxf/Drawing/GDIRenderer/GDIWindowContext.h"
#include "wx/generic/private/markuptext.h"

namespace kxf::UI::DataView
{
	static bool operator<(const Size& left, const Size& right)
	{
		return left.GetWidth() < right.GetWidth() || left.GetHeight() < right.GetHeight();
	}
	static bool operator>(const Size& left, const Size& right)
	{
		return left.GetWidth() > right.GetWidth() || left.GetHeight() > right.GetHeight();
	}

	static int GetTreeItemState(int flags)
	{
		int itemState = (flags & wxCONTROL_CURRENT) ? TREIS_HOT : TREIS_NORMAL;
		if (flags & wxCONTROL_SELECTED)
		{
			itemState = (flags & wxCONTROL_CURRENT) ? TREIS_HOTSELECTED : TREIS_SELECTED;
			if (!(flags & wxCONTROL_FOCUSED))
			{
				itemState = TREIS_SELECTEDNOTFOCUS;
			}
		}

		if (flags & wxCONTROL_DISABLED && !(flags & wxCONTROL_CURRENT))
		{
			itemState = TREIS_DISABLED;
		}
		return itemState;
	};
}

namespace kxf::UI::DataView::Markup
{
	class TextOnly final: public wxItemMarkupText
	{
		public:
			TextOnly(const String& markup)
				:wxItemMarkupText(markup)
			{
			}

		public:
			Size GetTextExtent(GDIContext& dc) const
			{
				return Measure(dc.ToWxDC());
			}
	};
	class WithMnemonics final: public wxMarkupText
	{
		public:
			WithMnemonics(const String& markup)
				:wxMarkupText(markup)
			{
			}

		public:
			Size GetTextExtent(GDIContext& dc) const
			{
				return Measure(dc.ToWxDC());
			}
	};
}

namespace kxf::UI::DataView::Markup
{
	using MarkupMode = Renderer::MarkupMode;
	template<MarkupMode t_Mode> auto Create(const String& string = {})
	{
		if constexpr(t_Mode == MarkupMode::TextOnly)
		{
			return TextOnly(string);
		}
		else if constexpr(t_Mode == MarkupMode::WithMnemonics)
		{
			return WithMnemonics(string);
		}
	}

	Size GetTextExtent(const wxMarkupTextBase& markup, GDIContext& dc)
	{
		return markup.Measure(dc.ToWxDC());
	}
	Size GetTextExtent(MarkupMode mode, GDIContext& dc, const String& string)
	{
		switch (mode)
		{
			case MarkupMode::TextOnly:
			{
				return Create<MarkupMode::TextOnly>(string).GetTextExtent(dc);
			}
			case MarkupMode::WithMnemonics:
			{
				return Create<MarkupMode::WithMnemonics>(string).GetTextExtent(dc);
			}
		};
		return Size(0, 0);
	}

	template<class T>
	void DrawText(T& markup, wxWindow* window, GDIContext& dc, const Rect& rect, int flags, wxEllipsizeMode ellipsizeMode)
	{
		if constexpr(std::is_same_v<T, WithMnemonics>)
		{
			markup.Render(dc.ToWxDC(), rect, flags);
		}
		else
		{
			markup.Render(window, dc.ToWxDC(), rect, flags, ellipsizeMode);
		}
	}

	void DrawText(MarkupMode mode, const String& string, wxWindow* window, GDIContext& dc, const Rect& rect, int flags, wxEllipsizeMode ellipsizeMode)
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

namespace kxf::UI::DataView
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
	Size RenderEngine::FromDIP(const Size& size) const
	{
		return m_Renderer.GetView()->FromDIP((wxSize)size);
	}

	size_t RenderEngine::FindFirstLineBreak(const String& string) const
	{
		for (size_t i = 0; i < string.size(); i++)
		{
			const wxChar c = string[i];
			if (c == wxS('\r') || c == wxS('\n'))
			{
				return i;
			}
		}
		return String::npos;
	}
	FlagSet<NativeWidgetFlag> RenderEngine::GetControlFlags(CellState cellState) const
	{
		const Column* column = m_Renderer.GetColumn();

		const auto& cellAttributes = m_Renderer.GetAttributes().Options();

		FlagSet<NativeWidgetFlag> flags;
		flags.Add(NativeWidgetFlag::Disabled, !cellAttributes.ContainsOption(CellOption::Enabled));
		flags.Add(NativeWidgetFlag::Editable, cellAttributes.ContainsOption(CellOption::Editable));
		flags.Add(NativeWidgetFlag::Selected|NativeWidgetFlag::Pressed, cellState.IsSelected());
		flags.Add(NativeWidgetFlag::Current|NativeWidgetFlag::Focused, column && (cellState.IsHotTracked() && column->IsHotTracked()));

		return flags;
	}
	String RenderEngine::StripMarkup(const String& markup) const
	{
		// Stub for now. Need proper mnemonics removal algorithm
		return wxControl::RemoveMnemonics(markup);
	}

	Size RenderEngine::GetTextExtent(const String& string) const
	{
		if (m_Renderer.CanDraw())
		{
			IGraphicsContext& gc = m_Renderer.GetGraphicsContext();
			return gc.GetTextExtent(string);
		}
		else
		{
			// No existing window context right now, create one to measure text
			IGraphicsRenderer& renderer = m_Renderer.GetGraphicsRenderer();
			auto gc = renderer.CreateMeasuringContext();

			gc->SetFont(renderer.CreateFont(m_Renderer.GetView()->GetFont()));
			return gc->GetTextExtent(string);
		}
	}
	Size RenderEngine::GetTextExtent(IGraphicsContext& gc, const String& string) const
	{
		const CellAttribute& attributes = m_Renderer.GetAttributes();

		if (m_Renderer.IsMarkupEnabled())
		{
			SizeF extent;
			gc.DrawGDI({0, 0, 1, 1}, [&](GDIContext& dc)
			{
				if (attributes.FontOptions().NeedDCAlteration())
				{
					dc.SetFont(attributes.GetEffectiveFont(dc.GetFont()));
				}
				extent = Markup::GetTextExtent(m_Renderer.m_MarkupMode, dc, string);
			});
			return extent;
		}
		else
		{
			auto GetEffectiveFontIfNeeded = [&]() -> Font
			{
				if (attributes.FontOptions().NeedDCAlteration())
				{
					if (auto font = gc.GetFont())
					{
						return attributes.GetEffectiveFont(font->ToFont());
					}
					else
					{
						return attributes.GetEffectiveFont(m_Renderer.GetView()->GetFont());
					}
				}
				return {};
			};
			auto MeasureString = [&](const String& text, const Font& font = {})
			{
				if (font)
				{
					auto gcFont = m_Renderer.GetGraphicsRenderer().CreateFont(font);
					return gc.GetTextExtent(text, *gcFont);
				}
				else
				{
					return gc.GetTextExtent(text);
				}
			};

			const size_t lineBreakPos = FindFirstLineBreak(string);
			if (lineBreakPos != String::npos)
			{
				return MeasureString(string.Left(lineBreakPos), GetEffectiveFontIfNeeded());
			}
			else
			{
				return MeasureString(string, GetEffectiveFontIfNeeded());
			}
		}
	}

	bool RenderEngine::DrawText(const Rect& cellRect, CellState cellState, const String& string, int offsetX)
	{
		if (m_Renderer.CanDraw() && !string.IsEmpty())
		{
			IGraphicsContext& gc = m_Renderer.GetGraphicsContext();
			return DrawText(gc, cellRect, cellState, string, offsetX);
		}
		return false;
	}
	bool RenderEngine::DrawText(IGraphicsContext& gc, const Rect& cellRect, CellState cellState, const String& string, int offsetX)
	{
		if (!string.IsEmpty())
		{
			const CellAttribute& attributes = m_Renderer.GetAttributes();

			Rect textRect = cellRect;
			textRect.X() += offsetX;
			textRect.Width() -= offsetX;

			int flags = 0;
			if (m_Renderer.IsMarkupWithMnemonicsEnabled() && attributes.Options().ContainsOption(CellOption::ShowAccelerators))
			{
				flags |= wxMarkupText::Render_ShowAccels;
			}

			if (m_Renderer.IsMarkupEnabled())
			{
				gc.DrawGDI({0, 0, 1, 1}, [&](GDIContext& dc)
				{
					Markup::DrawText(m_Renderer.m_MarkupMode, string, m_Renderer.GetView(), dc, textRect, flags, static_cast<wxEllipsizeMode>(m_Renderer.GetEllipsizeMode()));
				});
			}
			else
			{
				auto Ellipsize = [&]()
				{
					String ellipsizedText;
					gc.DrawGDI({0, 0, 1, 1}, [&](GDIContext& dc)
					{
						ellipsizedText = wxControl::Ellipsize(string, dc.ToWxDC(), static_cast<wxEllipsizeMode>(m_Renderer.GetEllipsizeMode()), textRect.GetWidth());
					});
					return ellipsizedText;
				};

				const size_t lineBreakPos = FindFirstLineBreak(string);
				if (lineBreakPos != String::npos)
				{
					gc.DrawText(Ellipsize().Left(lineBreakPos), textRect.GetPosition());
				}
				else
				{
					gc.DrawText(Ellipsize(), textRect.GetPosition());
				}
			}
			return true;
		}
		return false;
	}

	bool RenderEngine::DrawBitmap(const Rect& cellRect, CellState cellState, const BitmapImage& bitmap, int reservedWidth)
	{
		if (bitmap)
		{
			const CellAttribute& attributes = m_Renderer.GetAttributes();
			const Rect rect = {cellRect.GetPosition(), bitmap.GetSize()};
			const bool isEnabled = attributes.Options().ContainsOption(CellOption::Enabled);

			IGraphicsContext& gc = m_Renderer.GetGraphicsContext();
			gc.DrawTexture(isEnabled ? bitmap : bitmap.ConvertToDisabled(), rect);
			return true;
		}
		else if (reservedWidth > 0)
		{
			return true;
		}
		return false;
	}
	int RenderEngine::DrawBitmapWithText(const Rect& cellRect, CellState cellState, int offsetX, const String& text, const BitmapImage& bitmap, bool centerTextV, int reservedWidth)
	{
		if (bitmap || reservedWidth > 0)
		{
			DrawBitmap(Rect(cellRect.GetX() + offsetX, cellRect.GetY(), cellRect.GetWidth() - offsetX, cellRect.GetHeight()), cellState, bitmap, reservedWidth);
			offsetX += (reservedWidth > 0 ? reservedWidth : bitmap.GetSize().GetWidth()) + FromDIPX(GetInterTextSpacing());
		}
		if (!text.IsEmpty())
		{
			if (bitmap && centerTextV)
			{
				const Rect textRect = CenterTextInside(cellRect, GetTextExtent(text));
				DrawText(textRect, cellState, text, offsetX);
			}
			else
			{
				DrawText(cellRect, cellState, text, offsetX);
			}
		}
		return offsetX;
	}
	void RenderEngine::DrawProgressBar(const Rect& cellRect, CellState cellState, int value, int range, ProgressState state, Color* averageBackgroundColor)
	{
		IGraphicsContext& gc = m_Renderer.GetGraphicsContext();

		auto flags = GetControlFlags(cellState);
		flags.Add(NativeWidgetFlag::Paused, state == ProgressState::Paused);
		flags.Add(NativeWidgetFlag::Error, state == ProgressState::Error);

		IRendererNative::Get().DrawProgressMeter(m_Renderer.GetMainWindow(), gc, cellRect, value, range, flags);
	}

	Size RenderEngine::GetToggleSize() const
	{
		return wxRendererNative::Get().GetCheckBoxSize(m_Renderer.GetView());
	}
	Size RenderEngine::DrawToggle(IGraphicsContext& gc, const Rect& cellRect, CellState cellState, ToggleState toggleState, ToggleType toggleType)
	{
		auto flags = GetControlFlags(cellState);
		flags.Add(NativeWidgetFlag::Checkable);
		flags.Add(NativeWidgetFlag::Checked, toggleState == ToggleState::Checked);
		flags.Add(NativeWidgetFlag::Indeterminate, toggleState == ToggleState::Indeterminate);
		flags.Add(NativeWidgetFlag::Radio, toggleType != ToggleType::CheckBox && toggleState != ToggleState::Indeterminate);

		// Ensure that the check boxes always have at least the minimal required size,
		// otherwise DrawCheckBox() doesn't really work well. If this size is greater than
		// the cell size, the checkbox will be truncated but this is a lesser evil.
		View* view = m_Renderer.GetView();
		Rect toggleRect = cellRect;
		Size size = toggleRect.GetSize();
		size.IncTo(GetToggleSize());
		toggleRect.SetSize(size);

		IRendererNative::Get().DrawCheckBox(m_Renderer.GetView(), gc, toggleRect, flags);
		return toggleRect.GetSize();
	}
}
