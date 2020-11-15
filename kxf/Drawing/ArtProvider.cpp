#include "stdafx.h"
#include "ArtProvider.h"
#include "GDIRenderer.h"
#include "Image.h"
#include "ImageBundle.h"
#include "kxf/Utility/Container.h"
#include <wx/artprov.h>

namespace
{
	std::vector<std::pair<kxf::optional_ptr<kxf::IArtProvider>, wxArtProvider*>> g_Providers;

	class ArtProviderWrapper final: public wxArtProvider
	{
		private:
			kxf::IArtProvider& m_Provider;

		public:
			ArtProviderWrapper(kxf::IArtProvider& provider)
				:m_Provider(provider)
			{
			}

		public:
			wxBitmap CreateBitmap(const wxString& id, const wxString& clientID, const wxSize& size) override
			{
				return m_Provider.GetImage(kxf::String(id), clientID.IsEmpty() ? wxART_OTHER : clientID, size).ToBitmap().ToWxBitmap();
			}
			wxIconBundle CreateIconBundle(const wxString& id, const wxString& clientID) override
			{
				return m_Provider.GetImageBundle(kxf::String(id), clientID.IsEmpty() ? wxART_OTHER : clientID).ToWxIconBundle();
			}
	};
}

namespace kxf::ArtProviderClient
{
	const String ToolBar = wxART_TOOLBAR;
	const String Menu = wxART_MENU;
	const String List = wxART_LIST;
	const String Button = wxART_BUTTON;
	const String FrameIcon = wxART_FRAME_ICON;
	const String HelpBrowser = wxART_HELP_BROWSER;
	const String CommonDialog = wxART_CMN_DIALOG;
	const String MessageBox = wxART_MESSAGE_BOX;
}

namespace kxf::ArtProvider
{
	void PushProvider(optional_ptr<IArtProvider> artProvider)
	{
		if (artProvider)
		{
			// Remove any of such providers first
			RemoveProvider(*artProvider);

			// Make a wxWidgets wrapper for the new provider and add it
			auto wrapper = std::make_unique<ArtProviderWrapper>(*artProvider);
			g_Providers.emplace_back(std::make_pair(std::move(artProvider), wrapper.get()));
			wxArtProvider::Push(wrapper.release());
		}
	}
	void RemoveProvider(const IArtProvider& artProvider)
	{
		// Remove the provider from wxWidgets
		for (const auto& [provider, wrapper]: g_Providers)
		{
			if (provider.get() == &artProvider)
			{
				wxArtProvider::Delete(wrapper);
			}
		}

		// Remove it from out list
		Utility::Container::RemoveEachIf(g_Providers, [&](const auto& item)
		{
			return item.first.get() == &artProvider;
		});
	}
}

namespace kxf::ArtProvider
{
	GDIBitmap GetResource(const ResourceID& id, const String& clientID, const Size& size)
	{
		return wxArtProvider::GetBitmap(id.GetString(), clientID, size);
	}
	ImageBundle GetResourceBundle(const ResourceID& id, const String& clientID)
	{
		return wxArtProvider::GetIconBundle(id.GetString(), clientID);
	}

	ResourceID GetMessageBoxResourceIDs(StdIcon iconID)
	{
		return String(wxArtProvider::GetMessageBoxIconId(UI::ToWxStdIcon(iconID)));
	}
	GDIIcon GetMessageBoxResource(StdIcon iconID)
	{
		return wxArtProvider::GetMessageBoxIcon(UI::ToWxStdIcon(iconID));
	}
}
