# KxFramework
Support library for [Kortex Mod Manager](https://github.com/KerberX/Kortex-Mod-Manager).

# Build
You'll need [Visual Studio 2019](https://visualstudio.microsoft.com/) and [VCPkg](https://github.com/microsoft/vcpkg) package manager.

- Download and install **VCPkg**.
- Build **VCPkg** using its instructions (run `bootstrap-vcpkg.bat`, wait for build to complete and run `vcpkg.exe` from command line or PowerShell).
- Download and build following packages for both x86 (`:x86-windows-static-md`) and x64 (`:x64-windows-static-md`) targets.
	- [wxWidgets](https://www.wxwidgets.org) - `wxwidgets`.
	- [Boost](https://www.boost.org) - `boost`. The framework doesn't use Boost itself, only as a dependency for other libraries.
	- [LibCurl](https://curl.haxx.se) - `curl`. I'm using the following options: `curl[non-http,http2, winssl]`.
	- [WebSocket++](https://github.com/zaphoyd/websocketpp) - `websocketpp`.
	- [Gumbo HTML5 Parser](https://github.com/google/gumbo-parser) - `gumbo`.
	- [LibFFI](https://github.com/libffi/libffi) - `libffi`.
	- [LZ4](https://github.com/lz4/lz4) - `lz4`.
	- [JSON for Modern C++](https://github.com/nlohmann/json) - `nlohmann-json`.
	- [SimpleINI](https://github.com/brofield/simpleini) - `simpleini`.
	- [TinyXML2](https://github.com/leethomason/tinyxml2) - `tinyxml2`.
	
- Set this instance of **VCPkg** as default or use project-specific linking.
- Open KxFramework solution in Visual Studio and Build **Release** configuration for x86 and x64. 

# Configuration
KxFramework project file `KxFramework.vcxproj` is configured to use `x86-windows-static-md` and `x64-windows-static-md` triplets. If you're using any oher combination it might or might not be automatically detected by Visual Studio and then you'll need to chnage triplets in the project file as follows:
```xml
<PropertyGroup Label="Globals">
	...
	<VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static-md</VcpkgTriplet>
	<VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static-md</VcpkgTriplet>
</PropertyGroup>
```
Same thing applies to any project that will use KxFramework as a dependency. Consider reading [this article](https://devblogs.microsoft.com/cppblog/vcpkg-updates-static-linking-is-now-available) for more information.
