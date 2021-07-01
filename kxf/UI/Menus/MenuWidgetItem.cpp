#include "KxfPCH.h"
#include "MenuWidgetItem.h"
#include "MenuWidget.h"
#include "kxf/Drawing/BitmapImage.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/GDIRenderer/GDIBitmap.h"
#include "kxf/System/SystemInformation.h"
#include "WXUI/Menu.h"
#include "WXUI/MenuItem.h"
#include <wx/menu.h>
#include <wx/colour.h>
#include <Windows.h>

namespace kxf::Widgets
{
	// MenuWidgetItem
	void MenuWidgetItem::DoCreateWidget()
	{
		MenuWidget::AssociateWXMenuItem(*m_MenuItem, *this);
		m_MenuItem->SetOwnerDrawn();
	}
	bool MenuWidgetItem::DoDestroyWidget(bool releaseWX)
	{
		if (m_MenuItem)
		{
			auto menuItem = std::move(m_MenuItem);
			MenuWidget::DissociateWXMenuItem(*menuItem);

			if (releaseWX)
			{
				static_cast<void>(menuItem.release());
			}
			return true;
		}
		return false;
	}
	void MenuWidgetItem::OnWXMenuDestroyed()
	{
		DoDestroyWidget(true);
	}

	bool MenuWidgetItem::IsFirstItem() const
	{
		const auto& items = m_MenuItem->GetMenu()->GetMenuItems();
		if (!items.empty())
		{
			return items.front() == m_MenuItem.get();
		}
		return false;
	}
	bool MenuWidgetItem::IsLastItem() const
	{
		const auto& items = m_MenuItem->GetMenu()->GetMenuItems();
		if (!items.empty())
		{
			return items.back() == m_MenuItem.get();
		}
		return false;
	}

	Size MenuWidgetItem::OnMeasureItem(Size size) const
	{
		if (const wxWindow* window = m_MenuItem->GetWindow())
		{
			if (m_MenuItem->IsSeparator())
			{
				size.SetHeight(window->FromDIP(5));
				return size;
			}
			else
			{
				const int margin = window->FromDIP(6);
				const auto systemIconSize = System::GetMetric(SystemSizeMetric::IconSmall);

				size.SetHeight(systemIconSize.GetHeight() + margin);
				if (IsFirstItem() || IsLastItem())
				{
					size.Height() += window->FromDIP(4);
				}
				return size;
			}
		}
		return Size::UnspecifiedSize();
	}
	void MenuWidgetItem::OnDrawItem(std::shared_ptr<IGraphicsContext> gc, RectF rect, FlagSet<NativeWidgetFlag> flags)
	{
		if (wxWindow* window = m_MenuItem->GetWindow())
		{
			gc->SetAntialiasMode(AntialiasMode::None);
			gc->SetTextAntialiasMode(AntialiasMode::BestAvailable);
			gc->SetInterpolationQuality(InterpolationQuality::BestAvailable);

			auto backgroundBrush = gc->GetRenderer().CreateSolidBrush(System::GetColor(SystemColor::Menu));
			gc->Clear(*backgroundBrush);

			if (m_MenuItem->IsSeparator())
			{
				const int margin = window->FromDIP(8);
				rect.X() += margin;
				rect.Width() -= margin * 2;

				float x1 = rect.GetLeft();
				float x2 = rect.GetRight();
				float y = rect.GetY() + rect.GetHeight() / 2;

				auto pen = gc->GetRenderer().CreatePen(System::GetColor(SystemColor::AppWorkspace), 1);
				gc->DrawLine({x1, y}, {x2, y}, *pen);
			}
			else
			{
				IRendererNative& rendererNative = IRendererNative::Get();

				// Get parameters
				const int iconMargin = window->FromDIP(16);
				const auto systemIconSize = System::GetMetric(SystemSizeMetric::IconSmall);
				const String labelWithMnemonics = m_MenuItem->GetItemLabel();
				const String label = m_MenuItem->GetItemLabelText();
				const size_t acceleratorIndex = labelWithMnemonics.Find('&');
				const bool isFirstItem = IsFirstItem();
				const bool isLastItem = IsLastItem();

				// Calculate label rect
				if (isFirstItem || isLastItem)
				{
					const int edgeMargin = window->FromDIP(4);
					if (isFirstItem)
					{
						rect.Y() += edgeMargin;
						rect.Height() -= edgeMargin;
					}
					else if (isLastItem)
					{
						rect.Height() -= edgeMargin;
					}
				}

				RectF labelRect = rect;
				labelRect.Deflate(0, window->FromDIP(2));
				labelRect.Width() -= window->FromDIP(5);

				labelRect.X() += systemIconSize.GetWidth() + iconMargin;
				labelRect.Width() -= systemIconSize.GetWidth() * 2 + iconMargin;

				// Setup fonts and colors
				if (flags.Contains(NativeWidgetFlag::DefaultItem))
				{
					Font font = gc->GetFont()->ToFont();
					font.SetWeight(FontWeight::Bold);

					gc->SetFont(gc->GetRenderer().CreateFont(font));
				}
				if (flags.Contains(NativeWidgetFlag::Disabled))
				{
					auto brush = gc->GetFontBrush();
					if (object_ptr<IGraphicsSolidBrush> solidBrush; brush->QueryInterface(solidBrush))
					{
						solidBrush->SetColor(solidBrush->GetColor().MakeDisabled());
					}
					gc->SetBrush(brush);
					gc->SetFontBrush(brush);
				}

				// Draw icon/check/radio
				RectF iconSpaceRect = {rect.GetPosition(), SizeF(systemIconSize.GetWidth(), rect.GetHeight())};
				iconSpaceRect.Inflate(window->FromDIP(2), -1);
				iconSpaceRect.X() += iconMargin / 2;

				if (m_MenuItem->IsCheckable() && m_MenuItem->IsChecked())
				{
					FlagSet<NativeWidgetFlag> checkFlags = NativeWidgetFlag::Checkable|NativeWidgetFlag::Checked;
					checkFlags.Add(NativeWidgetFlag::Radio, m_MenuItem->IsRadio());

					auto checkRect = Rect(iconSpaceRect.GetPosition(), IRendererNative::Get().GetCheckBoxSize(window)).CenterIn(iconSpaceRect);
					rendererNative.DrawItemSelectionRect(window, *gc, iconSpaceRect, NativeWidgetFlag::Selected|NativeWidgetFlag::Current|NativeWidgetFlag::Focused);
					rendererNative.DrawCheckBox(window, *gc, checkRect, checkFlags);
				}
				else if (m_Icon)
				{
					RectF iconRect = iconSpaceRect.Clone().Deflate(1);
					if (flags.Contains(NativeWidgetFlag::Disabled))
					{
						gc->DrawTexture(m_Icon.ConvertToDisabled(), iconRect);
					}
					else
					{
						gc->DrawTexture(m_Icon, iconRect);
					}
				}

				// Draw the label
				if (!label.IsEmpty())
				{
					gc->DrawLabel(label, labelRect, Alignment::CenterVertical, acceleratorIndex);

					if (auto acceleratorText = labelWithMnemonics.AfterFirst('\t'); !acceleratorText.IsEmpty())
					{
						gc->DrawLabel(acceleratorText, labelRect, Alignment::CenterVertical|Alignment::Right);
					}
				}

				// Draw selection rect
				if (flags.Contains(NativeWidgetFlag::Selected))
				{
					FlagSet<NativeWidgetFlag> selectionFlags = NativeWidgetFlag::Selected|NativeWidgetFlag::Current;
					selectionFlags.Add(NativeWidgetFlag::Focused, !flags.Contains(NativeWidgetFlag::Disabled));

					rendererNative.DrawItemSelectionRect(window, *gc, rect, selectionFlags);
				}
			}
		}
	}

	MenuWidgetItem::MenuWidgetItem()
	{
	}
	MenuWidgetItem::~MenuWidgetItem()
	{
		DoDestroyWidget();
	}

	// --- IMenuWidgetItem ---
	// General
	std::shared_ptr<IMenuWidget> MenuWidgetItem::GetOwningMenu() const
	{
		return m_OwningMenu.lock();
	}

	std::shared_ptr<IMenuWidget> MenuWidgetItem::GetSubMenu() const
	{
		if (m_MenuItem)
		{
			if (auto subMenu = m_MenuItem->GetSubMenu())
			{
				object_ptr<IMenuWidget> menu;
				auto widget = Private::FindByWXObject(*subMenu);
				if (widget && widget->QueryInterface(menu))
				{
					return menu->LockMenuReference();
				}
			}
		}
		return nullptr;
	}
	void MenuWidgetItem::SetSubMenu(IMenuWidget& subMenu)
	{
		object_ptr<MenuWidget> menuWidget;
		if (m_MenuItem && subMenu.IsWidgetAlive() && subMenu.QueryInterface(menuWidget) && !menuWidget->m_IsAttached)
		{
			m_MenuItem->SetSubMenu(menuWidget->m_Menu.get());
		}
	}

	bool MenuWidgetItem::IsDefaultItem() const
	{
		if (m_MenuItem)
		{
			if (auto menu = m_OwningMenu.lock())
			{
				return menu->GetDefaultItem().get() == this;
			}
		}
		return false;
	}
	void MenuWidgetItem::SetDefaultItem()
	{
		if (m_MenuItem)
		{
			if (auto menu = m_OwningMenu.lock())
			{
				::SetMenuDefaultItem(reinterpret_cast<HMENU>(menu->GetHandle()), WXUI::Menu::WxIDToWin(m_MenuItem->GetId()), FALSE);
			}
		}
	}

	// Properties
	MenuWidgetItemType MenuWidgetItem::GetItemType() const
	{
		if (m_MenuItem)
		{
			if (m_MenuItem->IsSubMenu())
			{
				return MenuWidgetItemType::SubMenu;
			}

			switch (m_MenuItem->GetKind())
			{
				case wxITEM_NORMAL:
				{
					return MenuWidgetItemType::Regular;
				}
				case wxITEM_SEPARATOR:
				{
					return MenuWidgetItemType::Separator;
				}
				case wxITEM_CHECK:
				{
					return MenuWidgetItemType::CheckItem;
				}
				case wxITEM_RADIO:
				{
					return MenuWidgetItemType::RadioItem;
				}
			};
		}
		return MenuWidgetItemType::None;
	}

	String MenuWidgetItem::GetLabel(FlagSet<WidgetTextFlag> flags) const
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			if (flags.Contains(WidgetTextFlag::WithMnemonics))
			{
				return m_MenuItem->GetItemLabel();
			}
			else
			{
				return m_MenuItem->GetItemLabelText();
			}
		}
		return {};
	}
	void MenuWidgetItem::SetLabel(const String& label, FlagSet<WidgetTextFlag> flags)
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			m_MenuItem->SetItemLabel(label);
		}
	}

	String MenuWidgetItem::GetDescription() const
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			return m_MenuItem->GetHelp();
		}
		return {};
	}
	void MenuWidgetItem::SetDescription(const String& description)
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			m_MenuItem->SetHelp(description);
		}
	}

	WidgetID MenuWidgetItem::GetItemID() const
	{
		if (m_MenuItem)
		{
			if (m_MenuItem->IsSeparator())
			{
				return StdID::Separator;
			}
			else
			{
				return m_ItemID;
			}
		}
		return {};
	}
	void MenuWidgetItem::SetItemID(WidgetID id)
	{
		if (m_MenuItem && !m_MenuItem->IsSeparator())
		{
			m_ItemID = id;
		}
	}

	BitmapImage MenuWidgetItem::GetItemIcon() const
	{
		if (m_MenuItem)
		{
			return m_Icon;
		}
		return {};
	}
	void MenuWidgetItem::SetItemIcon(const BitmapImage& icon)
	{
		if (m_MenuItem)
		{
			m_Icon = icon;

			// Don't support checked bitmap for now 
			BitmapImage checked;

			if (m_MenuItem->IsCheckable())
			{
				if (checked)
				{
					m_MenuItem->SetBitmaps(m_Icon.ToGDIBitmap().ToWxBitmap(), checked.ToGDIBitmap().ToWxBitmap());
				}
				else
				{
					m_MenuItem->SetBitmap(m_Icon.ToGDIBitmap().ToWxBitmap(), false);
				}
			}
			else
			{
				m_MenuItem->SetBitmaps(m_Icon.ToGDIBitmap().ToWxBitmap(), checked.ToGDIBitmap().ToWxBitmap());
			}
		}
	}

	bool MenuWidgetItem::IsEnabled() const
	{
		if (m_MenuItem)
		{
			return m_MenuItem->IsEnabled();
		}
		return false;
	}
	void MenuWidgetItem::SetEnabled(bool enabled)
	{
		if (m_MenuItem)
		{
			m_MenuItem->Enable(enabled);
		}
	}

	bool MenuWidgetItem::IsChecked() const
	{
		if (m_MenuItem)
		{
			return m_MenuItem->IsChecked();
		}
		return false;
	}
	void MenuWidgetItem::SetChecked(bool checked)
	{
		if (m_MenuItem)
		{
			m_MenuItem->Check(checked);
		}
	}
}
