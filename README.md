# KxFramework
Support library for [Kortex Mod Manager](https://github.com/KerberX/Kortex-Mod-Manager)

# Dependencies
- [wxWidgets](https://www.wxwidgets.org) 3.1.2.
- [Boost](https://www.boost.org) 1.68. You can download precompiled binaries [here](https://sourceforge.net/projects/boost/files/boost-binaries).
- [OpenSSL](https://www.openssl.org) 1.1.0f ([precompiled binaries](https://www.npcglib.org/~stathis/blog/precompiled-openssl))
- [WebSocket++](https://github.com/zaphoyd/websocketpp) (header only library, included).
- [JSON for Modern C++](https://github.com/nlohmann/json) (header only library, included).
- [cURL](https://curl.haxx.se), (headers and compiled .lib files included).
- [Gumbo HTML5 Parser](https://github.com/google/gumbo-parser), (compiled .lib files included).
- [TinyXML2](https://github.com/leethomason/tinyxml2) (included).
- [SimpleINI](https://github.com/brofield/simpleini) (header only library, included).
- [LZ4](https://github.com/lz4/lz4) (headers and compiled .lib files included).
- [LibFFI](https://github.com/libffi/libffi) (headers and compiled .lib files included).

# Build (Visual Studio 2019)
- Install Boost and create environment variable **BOOST_ROOT** with value of Boost installation directory (like this: `C:\Program Files\Boost\boost_1_68_0`).
- Go to project *Properties* -> *C/C++* -> *General* -> *Additional include directories* and chnage include paths to required libraries.
- Same with .lib files (*Librarian* -> *General* -> *Additional library directories*).
- Build **Release** configuration for x86 and x64.
