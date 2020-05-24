#pragma once
#include "Common.h"
#include "kxf/General/IndexedEnum.h"
#include "kxf/System/ErrorCodeValue.h"

namespace kxf
{
	enum class HTTPStatus
	{
		Unknown = -1,

		Continue = 100,
		SwitchingProtocols = 101,
		Processing = 102,
		EarlyHints = 103,

		Success = 200,
		Created = 201,
		Accepted = 202,
		NonAuthoritativeInformation = 203,
		NoContent = 204,
		ResetContent = 205,
		PartialContent = 206,
		MultiStatus = 207,
		AlreadyReported = 208,
		IMUsed = 226,

		MultipleChoices = 300,
		MovedPermanently = 301,
		Found = 302,
		SeeOther = 303,
		NotModified = 304,
		UseProxy = 305,
		SwitchProxy = 306,
		TemporaryRedirect = 307,
		PermanentRedirect = 308,

		BadRequest = 400,
		Unauthorized = 401,
		PaymentRequired = 402,
		Forbidden = 403,
		NotFound = 404,
		MethodNotAllowed = 405,
		NotAcceptable = 406,
		ProxyAuthenticationRequired = 407,
		RequestTimeout = 408,
		Conflict = 409,
		Gone = 410,
		LengthRequired = 411,
		PreconditionFailed = 412,
		PayloadTooLarge = 413,
		URITooLong = 414,
		UnsupportedMediaType = 415,
		RangeNotSatisfiable = 416,
		ExpectationFailed = 417,
		ImATeapot = 418,
		UnprocessableEntity = 422,
		Locked = 423,
		FailedDependency = 424,
		UpgradeRequired = 426,
		PreconditionRequired = 428,
		TooManyRequests = 429,
		RequestHeaderFieldsTooLarge = 431,
		UnavailableForLegalReasons = 451,

		InternalServerError = 500,
		NotImplemented = 501,
		BadGateway = 502,
		ServiceUnavailable = 503,
		GatewayTimeout = 504,
		HTTPVersionNotSupported = 505,
		VariantAlsoNegotiates = 506,
		InsufficientStorage = 507,
		LoopDetected = 508,
		NotExtended = 510,
		NetworkAuthenticationRequired = 511
	};
}

namespace kxf
{
	struct HTTPStatusDef final: public IndexedEnumDefinition<HTTPStatusDef, HTTPStatus, StringView, true>
	{
		inline static constexpr TItem Items[] =
		{
			{HTTPStatus::Continue, wxS("Continue")},
			{HTTPStatus::SwitchingProtocols, wxS("SwitchingProtocols")},
			{HTTPStatus::Processing, wxS("Processing")},
			{HTTPStatus::EarlyHints, wxS("EarlyHints")},

			{HTTPStatus::Success, wxS("Success")},
			{HTTPStatus::Created, wxS("Created")},
			{HTTPStatus::Accepted, wxS("Accepted")},
			{HTTPStatus::NonAuthoritativeInformation, wxS("NonAuthoritativeInformation")},
			{HTTPStatus::NoContent, wxS("NoContent")},
			{HTTPStatus::ResetContent, wxS("ResetContent")},
			{HTTPStatus::PartialContent, wxS("PartialContent")},
			{HTTPStatus::MultiStatus, wxS("MultiStatus")},
			{HTTPStatus::AlreadyReported, wxS("AlreadyReported")},
			{HTTPStatus::IMUsed, wxS("IMUsed")},

			{HTTPStatus::MultipleChoices, wxS("MultipleChoices")},
			{HTTPStatus::MovedPermanently, wxS("MovedPermanently")},
			{HTTPStatus::Found, wxS("Found")},
			{HTTPStatus::SeeOther, wxS("SeeOther")},
			{HTTPStatus::NotModified, wxS("NotModified")},
			{HTTPStatus::UseProxy, wxS("UseProxy")},
			{HTTPStatus::SwitchProxy, wxS("SwitchProxy")},
			{HTTPStatus::TemporaryRedirect, wxS("TemporaryRedirect")},
			{HTTPStatus::PermanentRedirect, wxS("PermanentRedirect")},

			{HTTPStatus::BadRequest, wxS("BadRequest")},
			{HTTPStatus::Unauthorized, wxS("Unauthorized")},
			{HTTPStatus::PaymentRequired, wxS("PaymentRequired")},
			{HTTPStatus::Forbidden, wxS("Forbidden")},
			{HTTPStatus::NotFound, wxS("NotFound")},
			{HTTPStatus::MethodNotAllowed, wxS("MethodNotAllowed")},
			{HTTPStatus::NotAcceptable, wxS("NotAcceptable")},
			{HTTPStatus::ProxyAuthenticationRequired, wxS("ProxyAuthenticationRequired")},
			{HTTPStatus::RequestTimeout, wxS("RequestTimeout")},
			{HTTPStatus::Conflict, wxS("Conflict")},
			{HTTPStatus::Gone, wxS("Gone")},
			{HTTPStatus::LengthRequired, wxS("LengthRequired")},
			{HTTPStatus::PreconditionFailed, wxS("PreconditionFailed")},
			{HTTPStatus::PayloadTooLarge, wxS("PayloadTooLarge")},
			{HTTPStatus::URITooLong, wxS("URITooLong")},
			{HTTPStatus::UnsupportedMediaType, wxS("UnsupportedMediaType")},
			{HTTPStatus::RangeNotSatisfiable, wxS("RangeNotSatisfiable")},
			{HTTPStatus::ExpectationFailed, wxS("ExpectationFailed")},
			{HTTPStatus::ImATeapot, wxS("ImATeapot")},
			{HTTPStatus::UnprocessableEntity, wxS("UnprocessableEntity")},
			{HTTPStatus::Locked, wxS("Locked")},
			{HTTPStatus::FailedDependency, wxS("FailedDependency")},
			{HTTPStatus::UpgradeRequired, wxS("UpgradeRequired")},
			{HTTPStatus::PreconditionRequired, wxS("PreconditionRequired")},
			{HTTPStatus::TooManyRequests, wxS("TooManyRequests")},
			{HTTPStatus::RequestHeaderFieldsTooLarge, wxS("RequestHeaderFieldsTooLarge")},
			{HTTPStatus::UnavailableForLegalReasons, wxS("UnavailableForLegalReasons")},

			{HTTPStatus::InternalServerError, wxS("InternalServerError")},
			{HTTPStatus::NotImplemented, wxS("NotImplemented")},
			{HTTPStatus::BadGateway, wxS("BadGateway")},
			{HTTPStatus::ServiceUnavailable, wxS("ServiceUnavailable")},
			{HTTPStatus::GatewayTimeout, wxS("GatewayTimeout")},
			{HTTPStatus::HTTPVersionNotSupported, wxS("HTTPVersionNotSupported")},
			{HTTPStatus::VariantAlsoNegotiates, wxS("VariantAlsoNegotiates")},
			{HTTPStatus::InsufficientStorage, wxS("InsufficientStorage")},
			{HTTPStatus::LoopDetected, wxS("LoopDetected")},
			{HTTPStatus::NotExtended, wxS("NotExtended")},
			{HTTPStatus::NetworkAuthenticationRequired, wxS("NetworkAuthenticationRequired")},
		};
	};
}

namespace kxf
{
	// Indirectly implements 'ErrorCodeValue<>' static interface
	class HTTPStatusCode: public IndexedEnumValue<HTTPStatusDef, HTTPStatus::Unknown>
	{
		public:
			constexpr static ErrorCodeCategory GetCategory() noexcept
			{
				return ErrorCodeCategory::Unknown;
			}

		public:
			constexpr HTTPStatusCode() noexcept = default;
			constexpr HTTPStatusCode(HTTPStatus value) noexcept
				:IndexedEnumValue(value)
			{
			}
			constexpr HTTPStatusCode(TInt value) noexcept
				:HTTPStatusCode(static_cast<HTTPStatus>(value))
			{
			}

		public:
			constexpr bool IsSuccess() const noexcept
			{
				// [200, 300)
				return IsInRange(200, 299);
			}
			constexpr bool IsFail() const noexcept
			{
				// [400, +]
				return ToInt() >= 400;
			}
			constexpr bool IsInformation() const noexcept
			{
				// [100, 200)
				return IsInRange(100, 199);
			}
			constexpr bool IsRedirection() const noexcept
			{
				// [300, 400)
				return IsInRange(300, 399);
			}
			constexpr bool IsClientError() const noexcept
			{
				// [400, 500)
				return IsInRange(400, 499);
			}
			constexpr bool IsServerError() const noexcept
			{
				// [500, 600)
				return IsInRange(500, 599);
			}

			String ToString() const
			{
				return String::FromView(IndexedEnumValue::ToString());
			}
			String GetMessage() const
			{
				// TODO: Add (localized if possible) error messages.
				return {};
			}

		public:
			HTTPStatusCode& AddFlag(TEnum value) noexcept = delete;
			HTTPStatusCode& RemoveFlag(TEnum value) noexcept = delete;
			HTTPStatusCode& ModFlag(TEnum value, bool condition) noexcept = delete;
			bool HasFlag(TEnum value) const noexcept = delete;
			bool HasSpecifiedFlagOnly(TEnum value) const noexcept = delete;

			constexpr explicit operator bool() const noexcept
			{
				return IsSuccess();
			}
			constexpr bool operator!() const noexcept
			{
				return IsFail();
			}
	};
}
