# kxf-framework
Formerly KxFramework, initially a support library for [Kortex Mod Manager](https://github.com/Karandra/Kortex-Mod-Manager) and a general use application framework. 

# Build
## For development
You'll need [Visual Studio 2022](https://visualstudio.microsoft.com) and [VCPkg](https://github.com/microsoft/vcpkg) package manager.

- Download and install **VCPkg**.
- Build **VCPkg** using its instructions (run `bootstrap-vcpkg.bat`, wait for build to complete and run `vcpkg.exe` from the terminal).
- Copy `VCPkg` folder contents to your VCPkg installation.
- Download and build following packages for both x86 (`kxf-default-x86`) and x64 (`kxf-default-x64`) targets.
	- [wxWidgets](https://www.wxwidgets.org) - `wxwidgets[fonts,media,sound,webview]`
	- [Sciter-JS](https://github.com/c-smile/sciter-js-sdk) - `sciter-js` (see notes).
	- [libcurl](https://curl.haxx.se) - `curl[non-http,http2,openssl]`
	- [OpenSSL](https://www.openssl.org) - `openssl` and `openssl-windows`. Should be downloaded as a dependency for libcurl, but it's used directly as well.
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
	- [Scintilla](https://www.scintilla.org) - `scintilla`. It's already built-in into wxWidgets, but kxf-framework uses its headers directly and they're not available from wxWidgets includes.
	- [lunasvg](https://github.com/sammycage/lunasvg) - `lunasvg`. Use the v1.4.2 VCPkg port provided in this repository for it. The new version has a different API and is not compatible at the time.
	- [Microsoft Detours](https://github.com/microsoft/Detours) - `detours`

- Set this instance of VCPkg as the default (`vcpkg integrate install`) or use project-specific linking (a NuGet package generated from a VCPkg installation: `vcpkg integrate project`).
- Open kxf-framework solution in Visual Studio and Build **Release** configuration for x86 and x64. You can build **Debug** configuration as well, but it's not used most of the time.

## As a dependency
The framework can be built as a VCPkg package. You need to make a custom triplet using provided `kxf-default-[x86|x64].cmake` template file (or just use this triplet as is).

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
Sciter-JS can't be built using the default `windows-static-md` triplet because it's a dynamic library, but the  framework itself is static. Modified triplet configuration is provided in `VCPkg\triplets\community` folder. The old Sciter isn't used anymore.

# Configuration

**This paragraph is a bit outdated since Visual Studio now has a VCPkg property page in project configuration window**

Main project file `kxf-framework.vcxproj` isn't configured to use any triplets. Suitable triplet might or might not be automatically detected by Visual Studio so you'll need to change triplets in the `.vcxproj` (or even better `.vcxproj.user`) file as follows (example for `kxf-default-[x86|x64]`):

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

Same thing applies to any project that will use kxf-framework as a dependency. Consider reading [this article](https://devblogs.microsoft.com/cppblog/vcpkg-updates-static-linking-is-now-available) for more information.

# Projects using this framework
It's all my own projects to be honest.
- [Kortex Mod Manager](https://github.com/Karandra/Kortex-Mod-Manager) uses an old version of the framework and its development is currently frozen.
- [KxVirtualFileSystem](https://github.com/Karandra/KxVirtualFileSystem) is the virtual file system module Kortex Mod Manager uses. It currently doesn't use the framework, but it will soon!
- [Bethesda Module ShellView](https://github.com/Karandra/Bethesda-Module-ShellView) - a small shell integration app to show basic Bethesda plugin file (.esp/.esm/.esl) header information in Windows Explorer's property window.
- [Bethesda Archive Commander](https://github.com/Karandra/Bethesda-Archive-Commander) - an archiver application for Bethesda game archives (.bsa). It only supports BSA format up to Skyrim LE. It also uses a quite old framework version as I never got to update it.
- [xSE PluginPreloader](https://github.com/Karandra/xSE-PluginPreloader) is an utility for Bethesda games to load some XSE plugins before game's own static initializers were run.
- [PrivateProfileRedirector](https://github.com/Karandra/PrivateProfileRedirector) - a quality-of-life mod to reduce some Bethesda games startup time due to its inefficient configuration loading methods.

Most of them, one way or another, are related to Bethesda games too...

# Future plans
- Fully implement **Sciter** UI module for a more modern and flexible GUI library backend. Or find something with better documentation.
- Eventually get rid of **wxWidgets** and native controls or, at least, hide wxWidgets inside the implementation part. Removing wxWidgets altogether would require a fully ready to use Sciter module and many other changes.
- Either get rid of the `Enumerator` code (and revert all the code changes for it) or rewrite it to use C++20  coroutines. While being an interesting experiment, in its current state it's no better than a simple callback function while consuming a lot of maintenance time whenever I have to use it.
  - Partially mitigated by a new `CallbackFunction` class and adapters for it, but it's not used in many places yet.
- Incorporate any useful code from [KxVirtualFileSystem](https://github.com/Karandra/KxVirtualFileSystem) and [PrivateProfileRedirector](https://github.com/Karandra/PrivateProfileRedirector) code bases and make them use this framework.
  - PrivateProfileRedirector is done now.
- Update old code to use newer C++ features.
