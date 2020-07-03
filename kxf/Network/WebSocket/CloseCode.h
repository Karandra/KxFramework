#pragma once

namespace kxf::WebSocket
{
	enum class CloseCode: uint32_t
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
