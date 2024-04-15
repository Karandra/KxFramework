# KxFramework
Support library for [Kortex Mod Manager](https://github.com/Karandra/Kortex-Mod-Manager). Sometimes is used for my other projects since the stuff that is packed inside is so much useful.

# Build
## For development
You'll need [Visual Studio 2019](https://visualstudio.microsoft.com) and [VCPkg](https://github.com/microsoft/vcpkg) package manager.

- Download and install **VCPkg**.
- Build **VCPkg** using its instructions (run `bootstrap-vcpkg.bat`, wait for build to complete and run `vcpkg.exe` from the terminal).
- Copy `VCPkg` folder contents to your VCPkg installation.
- Download and build following packages for both x86 (`kxf-default-x86`) and x64 (`kxf-default-x64`) targets.
	- [wxWidgets](https://www.wxwidgets.org) - `wxwidgets[fonts,media,sound,webview]`
	- [Sciter-JS](https://github.com/c-smile/sciter-js-sdk) - `sciter-js` (see notes).
	- [libcurl](https://curl.haxx.se) - `curl[non-http,http2,openssl]`
	- [OpenSSL](https://www.openssl.org) - `openssl` and `openssl-windows`. Should be downloaded as a dependency for libcurl but it's used directly as well.
	- [WebSocket++](https://github.com/zaphoyd/websocketpp) - `websocketpp`
	- [Gumbo HTML5 Parser](https://github.com/google/gumbo-parser) - `gumbo`
	- [libffi](https://github.com/libffi/libffi) - `libffi`
	- [LZ4](https://github.com/lz4/lz4) - `lz4`
	- [7-Zip SDK](https://www.7-zip.org) - `7zip`
	- [JSON for Modern C++](https://github.com/nlohmann/json) - `nlohmann-json`
	- [SimpleINI](https://github.com/brofield/simpleini) - `simpleini`
	- [TinyXML2](https://github.com/leethomason/tinyxml2) - `tinyxml2`
	- [xxHash](https://github.com/Cyan4973/xxHash) - `xxhash`
	- [URIParser](https://github.com/uriparser/uriparser) - `uriparser`
	- [Scintilla](https://www.scintilla.org) - `scintilla`. It's already built-in into wxWidgets but KxFramework uses its headers directly and they're not available from wxWidgets includes.
	- [lunasvg](https://github.com/sammycage/lunasvg) - `lunasvg`. Use the v1.4.2 VCPkg port provided in this repository for it. The new version has a different API and is not compatible at the time.

- Set this instance of VCPkg as the default (`vcpkg integrate install`) or use project-specific linking (a NuGet package generated from a VCPkg installation: `vcpkg integrate project`).
- Open KxFramework solution in Visual Studio and Build **Release** configuration for x86 and x64. You can build **Debug** configuration as well but it's not used most of the time.

## As a dependency
KxFramework can be built as a VCPkg package. You need to make a custom triplet using provided `kxf-default-[x86|x64].cmake` template file (or just use this triplet as is).

If you're using precompiled headers in your project don't forget to include `kxf/pch.hpp` file in your project's precompiled header file.
```cpp
#pragma once

// Unfortunately wxWidgets uses deprecated insecure functions
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <kxf/pch.hpp>

// Additional headers here
```

### Build notes
Sciter-JS can't be built using the default `windows-static-md` triplet because it's a dynamic library but KxFramework itself is static. Modified triplet configuration is provided in `VCPkg\triplets\community` folder. The old Sciter isn't used anymore.

# Configuration

**This paragraph is a bit outdated since Visual Studio now has a VCPkg property page in project configuration window**

KxFramework project file `KxFramework.vcxproj` isn't configured to use any triplets. Suitable triplet might or might not be automatically detected by Visual Studio so you'll need to change triplets in the `.vcxproj` (or even better `.vcxproj.user`) file as follows (example for `kxf-default-[x86|x64]`):

```xml
<PropertyGroup Label="Vcpkg">
	...
	<VcpkgTriplet Condition="'$(Platform)'=='Win32'">kxf-default-x86</VcpkgTriplet>
	<VcpkgTriplet Condition="'$(Platform)'=='x64'">kxf-default-x64</VcpkgTriplet>
</PropertyGroup>
```
Entire `.vcxproj.user` content for example.
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
	<PropertyGroup Label="Vcpkg">
		<VcpkgTriplet Condition="'$(Platform)'=='Win32'">kxf-default-x86</VcpkgTriplet>
		<VcpkgTriplet Condition="'$(Platform)'=='x64'">kxf-default-x64</VcpkgTriplet>
	</PropertyGroup>
</Project>
```

Same thing applies to any project that will use KxFramework as a dependency. Consider reading [this article](https://devblogs.microsoft.com/cppblog/vcpkg-updates-static-linking-is-now-available) for more information.

# Future plans
- Fully implement **Sciter** UI module.
- Eventually get rid of **wxWidgets** and native controls. Requires fully ready to use Sciter module and many other changes.
- ~~Rewrite CURL wrapper to make it async.~~ [Done](https://github.com/Karandra/KxFramework/commit/064d181dbf0be8817df1057a55a37bdd7d2166b7)
