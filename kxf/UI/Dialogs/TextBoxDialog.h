#pragma once
#include "kxf/UI/Common.h"
#include "StdDialog.h"
#include "kxf/UI/Controls/TextBox.h"
#include "kxf/UI/Controls/StyledTextBox.h"

namespace kxf::UI
{
	enum class TextBoxDialogStyle: uint32_t
	{
		None = 0,

		Multiline = 1 << 0,
		ReadOnly = 1 << 1,
		Password = 1 << 2,
		Styled = 1 << 3
	};
}
namespace kxf
{
	KxFlagSet_Declare(UI::TextBoxDialogStyle);
	KxFlagSet_Extend(UI::TextBoxDialogStyle, UI::DialogStyle);
}

namespace kxf::UI
{
	class KX_API TextBoxDialog: public StdDialog
	{
		public:
			static constexpr FlagSet<TextBoxDialogStyle> DefaultStyle = TextBoxDialogStyle::None|*StdDialog::DefaultStyle;
			static constexpr int DefaultWidth = 300;
			static constexpr int DefaultMLWidth = 450;
			static constexpr int DefaultMLHeight = 200;

		private:
			wxControl* m_View = nullptr;
			FlagSet<TextBoxDialogStyle> m_Options = TextBoxDialogStyle::None;

		private:
			wxOrientation GetViewLabelSizerOrientation() const override;
			wxOrientation GetWindowResizeSide() const override;
			bool IsEnterAllowed(wxKeyEvent& event, WidgetID* idOut = nullptr) const override;

			bool IsStyledTextBox() const
			{
				return m_Options & TextBoxDialogStyle::Styled;
			}

		public:
			TextBoxDialog() = default;
			TextBoxDialog(wxWindow* parent,
						  wxWindowID id,
						  const String& caption,
						  const Point& pos = Point::UnspecifiedPosition(),
						  const Size& size = Size::UnspecifiedSize(),
						  FlagSet<StdButton> buttons = DefaultButtons,
						  FlagSet<TextBoxDialogStyle> style = DefaultStyle
			)
			{
				Create(parent, id, caption, pos, size, buttons, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& caption,
						const Point& pos = Point::UnspecifiedPosition(),
						const Size& size = Size::UnspecifiedSize(),
						FlagSet<StdButton> buttons = DefaultButtons,
						FlagSet<TextBoxDialogStyle> style = DefaultStyle
			);

		public:
			wxWindow* GetDialogMainCtrl() const override
			{
				return m_View;
			}
			int GetViewSizerProportion() const override
			{
				return IsMultiLine() ? 1 : 0;
			}
		
			TextBox* GetTextBox() const
			{
				if (!IsStyledTextBox())
				{
					return static_cast<TextBox*>(m_View);
				}
				return nullptr;
			}
			StyledTextBox* GetStyledTextBox() const
			{
				if (IsStyledTextBox())
				{
					return static_cast<StyledTextBox*>(m_View);
				}
				return nullptr;
			}

			String GetValue() const
			{
				if (GetTextBox())
				{
					return GetTextBox()->GetValue();
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->GetValue();
				}
				return {};
			}
			void SetValue(const String& value)
			{
				if (GetTextBox())
				{
					GetTextBox()->SetValue(value);
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetValue(value);
				}
			}
		
			bool IsMultiLine() const
			{
				if (GetTextBox())
				{
					return GetTextBox()->IsMultiLine();
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->IsMultiline();
				}
				return false;
			}
			void SetMultiLine(bool value)
			{
				if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetMultiline(value);
				}
			}
		
			bool IsEditable() const
			{
				if (GetTextBox())
				{
					return GetTextBox()->IsEditable();
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->IsEditable();
				}
				return false;
			}
			void SetEditable(bool value)
			{
				if (GetTextBox())
				{
					GetTextBox()->SetEditable(value);
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetEditable(value);
				}
			}
		
			bool SetHint(const String& value)
			{
				if (GetTextBox())
				{
					return GetTextBox()->SetHint(value);
				}
				else if (GetStyledTextBox())
				{
					return GetStyledTextBox()->SetHint(value);
				}
				return false;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(TextBoxDialog);
	};
}
