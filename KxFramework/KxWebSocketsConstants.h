/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once

enum KxWebSocketCloseCode
{
	KxWS_CLOSE_NORMAL = 1000, // Successful operation, regular socket shutdown
	KxWS_CLOSE_GOING_AWAY = 1001, // One of the socket endpoints is exiting
	KxWS_CLOSE_PROTOCOL_ERROR = 1002, // Error in one of the endpoints while processing a known message type
	KxWS_CLOSE_UNSUPPORTED = 1003, // Endpoint received unsupported data type (text/binary)
	KxWS_CLOSE_NO_STATUS = 1005, // Expected close status, received none
	KxWS_CLOSE_ABNORMAL = 1006, // No close code frame has been received
	KxWS_CLOSE_UNSUPPORTED_DATA = 1007, // Endpoint received inconsistent message (e.g. non-UTF8 data within a string)
	KxWS_CLOSE_POLICY_VIOLATION = 1008, // Endpoint policy was violated, is a generic code used when codes 1003 and 1009 aren't suitable
	KxWS_CLOSE_TOO_LARGE = 1009, // Data frame size is too large
	KxWS_CLOSE_MISSING_EXTENSION = 1010, // Client asked for extension that server didn't reply with
	KxWS_CLOSE_INTERNAL_ERROR = 1011, // Internal server error while operating
	KxWS_CLOSE_SERVICE_RESTART = 1012, // Server/service is restarting
	KxWS_CLOSE_TRY_AGAIN_LATER = 1013, // Try Again Later code; temporary server condition forced to block client's request
	KxWS_CLOSE_TLS_HANDSHAKE_FAIL = 1015, // TLS handshake failure
};
