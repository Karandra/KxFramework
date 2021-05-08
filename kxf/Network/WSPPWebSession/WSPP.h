#pragma once
#include "Common.h"

#include <websocketpp/version.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

namespace kxf
{
	enum class WSPPCloseCode: uint32_t
	{
		Normal = 1000, // Successful operation, regular socket shutdown
		GoingAway = 1001, // One of the socket endpoints is exiting
		ProtocolError = 1002, // Error in one of the endpoints while processing a known message type
		Unsupported = 1003, // Endpoint received unsupported data type (text/binary)
		NoStatus = 1005, // Expected close status, received none
		Abnormal = 1006, // No close code frame has been received
		UnsupportedData = 1007, // Endpoint received inconsistent message (e.g. non-UTF8 data within a string)
		PolicyViolation = 1008, // Endpoint policy was violated, is a generic code used when codes 1003 and 1009 aren't suitable
		TooLarge = 1009, // Data frame size is too large
		MissingExtension = 1010, // Client asked for extension that server didn't reply with
		InternalError = 1011, // Internal server error while operating
		ServiceRestart = 1012, // Server/service is restarting
		TryAgainLater = 1013, // Try Again Later code; temporary server condition forced to block client's request
		TLSHandshakeFail = 1015, // TLS handshake failure
	};
}
namespace kxf::Private
{
	class WSPPTypes
	{
		protected:
			using TClient = websocketpp::client<websocketpp::config::asio_tls_client>;
			using TConnection = typename TClient::connection_type;
			using TConnectionHandle = websocketpp::connection_hdl;

			using TMessage = websocketpp::config::asio_tls_client::message_type;
			using TConnectionState = websocketpp::session::state::value;
			using TFrameOpCode = websocketpp::frame::opcode::value;
			using TCloseStatus = websocketpp::close::status::value;
			using TErrorCode = websocketpp::lib::error_code;
			using TSSLContext = boost::asio::ssl::context;
	};
}
