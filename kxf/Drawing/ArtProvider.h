#pragma once
#include "Common.h"
#include "IArtProvider.h"
#include "kxf/General/OptionalPtr.h"
#include "kxf/UI/StdIcon.h"

namespace kxf
{
	class GDIBitmap;
	class GDIIcon;
	class BitmapImage;
	class ImageBundle;
}

namespace kxf::ArtProviderClient
{
	extern const String ToolBar;
	extern const String Menu;
	extern const String List;
	extern const String Button;
	extern const String FrameIcon;
	extern const String HelpBrowser;
	extern const String CommonDialog;
	extern const String MessageBox;
}

namespace kxf::ArtProvider
{
	KX_API void PushProvider(optional_ptr<IArtProvider> artProvider);
	KX_API void RemoveProvider(const IArtProvider& artProvider);
}

namespace kxf::ArtProvider
{
	KX_API GDIBitmap GetResource(const ResourceID& id, const String& clientID = {}, const Size& size = Size::UnspecifiedSize());
	KX_API ImageBundle GetResourceBundle(const ResourceID& id, const String& clientID = {});

	KX_API ResourceID GetMessageBoxResourceIDs(StdIcon iconID);
	KX_API GDIIcon GetMessageBoxResource(StdIcon iconID);
}
