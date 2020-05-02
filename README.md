# KxFramework
Support library for [Kortex Mod Manager](https://github.com/KerberX/Kortex-Mod-Manager).

# Build
## For development
You'll need [Visual Studio 2019](https://visualstudio.microsoft.com/) and [VCPkg](https://github.com/microsoft/vcpkg) package manager.

- Download and install **VCPkg**.
- Build **VCPkg** using its instructions (run `bootstrap-vcpkg.bat`, wait for build to complete and run `vcpkg.exe` from command line or PowerShell).
- Download and build following packages for both x86 (`x86-windows-static-md`) and x64 (`x64-windows-static-md`) targets.
	- [wxWidgets](https://www.wxwidgets.org) - `wxwidgets`.
	- [Sciter](https://github.com/c-smile/sciter-sdk) - `sciter` (see notes).
	- [Boost](https://www.boost.org) - `boost`. The framework doesn't use Boost itself, only as a dependency for other libraries.
	- [LibCurl](https://curl.haxx.se) - `curl`. I'm using the following options: `curl[non-http,http2,openssl]`.
	- [OpenSSL](https://www.openssl.org) - `openssl` and `openssl-windows`. Should be downloaded as a dependency for LibCurl but it's used directly as well.
	- [WebSocket++](https://github.com/zaphoyd/websocketpp) - `websocketpp`.
	- [Gumbo HTML5 Parser](https://github.com/google/gumbo-parser) - `gumbo`.
	- [LibFFI](https://github.com/libffi/libffi) - `libffi`.
	- [LZ4](https://github.com/lz4/lz4) - `lz4`.
	- [JSON for Modern C++](https://github.com/nlohmann/json) - `nlohmann-json`.
	- [SimpleINI](https://github.com/brofield/simpleini) - `simpleini`.
	- [TinyXML2](https://github.com/leethomason/tinyxml2) - `tinyxml2`.
	
- Set this instance of **VCPkg** as default or use project-specific linking.
- Open KxFramework solution in Visual Studio and Build **Release** configuration for x86 and x64. 

## As a dependency
KxFramework can be built as a **VCPkg** pacckage. Install **VCPkg** as usual and copy `VCPkg` folder contents into your **VCPkg** installation and build `kxframework` port (preferrably with `[x86|x64]-windows-static-md` triplet).

### Build notes
Sciter can't be built using the default `windows-static-md` triplet because it's a dynamic library. Modified triplet configuration  is provided in `VCPkg\triplets\community` folder.

# Configuration
KxFramework project file `KxFramework.vcxproj` isn't configured to use any triplets. Suitable triplet might or might not be automatically detected by Visual Studio so you'll need to change triplets in the `.vcxproj` (or even better `.vcxproj.user`) file as follows (example for `x86-windows-static-md` and `x64-windows-static-md`):
```xml
<PropertyGroup Label="Globals">
	...
	<VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static-md</VcpkgTriplet>
	<VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static-md</VcpkgTriplet>
</PropertyGroup>
```
Entire `.vcxproj.user` content for example.
```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
	<PropertyGroup Label="Globals">
		<VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static-md</VcpkgTriplet>
		<VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static-md</VcpkgTriplet>
	</PropertyGroup>
</Project>
```

Same thing applies to any project that will use KxFramework as a dependency. Consider reading [this article](https://devblogs.microsoft.com/cppblog/vcpkg-updates-static-linking-is-now-available) for more information.
