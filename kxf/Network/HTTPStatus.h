#pragma once
#include "Common.h"
#include "kxf/General/IErrorCode.h"
#include "kxf/General/IndexedEnum.h"

namespace kxf
{
	enum class HTTPStatusCode: uint32_t
	{
		Unknown = std::numeric_limits<uint32_t>::max(),

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
	struct HTTPStatusCodeDef final: public IndexedEnumDefinition<HTTPStatusCodeDef, HTTPStatusCode, StringView, true>
	{
		inline static constexpr TItem Items[] =
		{
			{HTTPStatusCode::Continue, wxS("Continue")},
			{HTTPStatusCode::SwitchingProtocols, wxS("SwitchingProtocols")},
			{HTTPStatusCode::Processing, wxS("Processing")},
			{HTTPStatusCode::EarlyHints, wxS("EarlyHints")},

			{HTTPStatusCode::Success, wxS("Success")},
			{HTTPStatusCode::Created, wxS("Created")},
			{HTTPStatusCode::Accepted, wxS("Accepted")},
			{HTTPStatusCode::NonAuthoritativeInformation, wxS("NonAuthoritativeInformation")},
			{HTTPStatusCode::NoContent, wxS("NoContent")},
			{HTTPStatusCode::ResetContent, wxS("ResetContent")},
			{HTTPStatusCode::PartialContent, wxS("PartialContent")},
			{HTTPStatusCode::MultiStatus, wxS("MultiStatus")},
			{HTTPStatusCode::AlreadyReported, wxS("AlreadyReported")},
			{HTTPStatusCode::IMUsed, wxS("IMUsed")},

			{HTTPStatusCode::MultipleChoices, wxS("MultipleChoices")},
			{HTTPStatusCode::MovedPermanently, wxS("MovedPermanently")},
			{HTTPStatusCode::Found, wxS("Found")},
			{HTTPStatusCode::SeeOther, wxS("SeeOther")},
			{HTTPStatusCode::NotModified, wxS("NotModified")},
			{HTTPStatusCode::UseProxy, wxS("UseProxy")},
			{HTTPStatusCode::SwitchProxy, wxS("SwitchProxy")},
			{HTTPStatusCode::TemporaryRedirect, wxS("TemporaryRedirect")},
			{HTTPStatusCode::PermanentRedirect, wxS("PermanentRedirect")},

			{HTTPStatusCode::BadRequest, wxS("BadRequest")},
			{HTTPStatusCode::Unauthorized, wxS("Unauthorized")},
			{HTTPStatusCode::PaymentRequired, wxS("PaymentRequired")},
			{HTTPStatusCode::Forbidden, wxS("Forbidden")},
			{HTTPStatusCode::NotFound, wxS("NotFound")},
			{HTTPStatusCode::MethodNotAllowed, wxS("MethodNotAllowed")},
			{HTTPStatusCode::NotAcceptable, wxS("NotAcceptable")},
			{HTTPStatusCode::ProxyAuthenticationRequired, wxS("ProxyAuthenticationRequired")},
			{HTTPStatusCode::RequestTimeout, wxS("RequestTimeout")},
			{HTTPStatusCode::Conflict, wxS("Conflict")},
			{HTTPStatusCode::Gone, wxS("Gone")},
			{HTTPStatusCode::LengthRequired, wxS("LengthRequired")},
			{HTTPStatusCode::PreconditionFailed, wxS("PreconditionFailed")},
			{HTTPStatusCode::PayloadTooLarge, wxS("PayloadTooLarge")},
			{HTTPStatusCode::URITooLong, wxS("URITooLong")},
			{HTTPStatusCode::UnsupportedMediaType, wxS("UnsupportedMediaType")},
			{HTTPStatusCode::RangeNotSatisfiable, wxS("RangeNotSatisfiable")},
			{HTTPStatusCode::ExpectationFailed, wxS("ExpectationFailed")},
			{HTTPStatusCode::ImATeapot, wxS("ImATeapot")},
			{HTTPStatusCode::UnprocessableEntity, wxS("UnprocessableEntity")},
			{HTTPStatusCode::Locked, wxS("Locked")},
			{HTTPStatusCode::FailedDependency, wxS("FailedDependency")},
			{HTTPStatusCode::UpgradeRequired, wxS("UpgradeRequired")},
			{HTTPStatusCode::PreconditionRequired, wxS("PreconditionRequired")},
			{HTTPStatusCode::TooManyRequests, wxS("TooManyRequests")},
			{HTTPStatusCode::RequestHeaderFieldsTooLarge, wxS("RequestHeaderFieldsTooLarge")},
			{HTTPStatusCode::UnavailableForLegalReasons, wxS("UnavailableForLegalReasons")},

			{HTTPStatusCode::InternalServerError, wxS("InternalServerError")},
			{HTTPStatusCode::NotImplemented, wxS("NotImplemented")},
			{HTTPStatusCode::BadGateway, wxS("BadGateway")},
			{HTTPStatusCode::ServiceUnavailable, wxS("ServiceUnavailable")},
			{HTTPStatusCode::GatewayTimeout, wxS("GatewayTimeout")},
			{HTTPStatusCode::HTTPVersionNotSupported, wxS("HTTPVersionNotSupported")},
			{HTTPStatusCode::VariantAlsoNegotiates, wxS("VariantAlsoNegotiates")},
			{HTTPStatusCode::InsufficientStorage, wxS("InsufficientStorage")},
			{HTTPStatusCode::LoopDetected, wxS("LoopDetected")},
			{HTTPStatusCode::NotExtended, wxS("NotExtended")},
			{HTTPStatusCode::NetworkAuthenticationRequired, wxS("NetworkAuthenticationRequired")},
		};
	};
}

namespace kxf
{
	class HTTPStatus final: public RTTI::ExtendInterface<HTTPStatus, IErrorCode>, public IndexedEnumValue<HTTPStatusCodeDef, HTTPStatusCode::Unknown>
	{
		KxRTTI_DeclareIID(HTTPStatus, {0xc9e3d90, 0xaf81, 0x48d8, {0x90, 0x25, 0xca, 0x15, 0x71, 0xec, 0x3d, 0x59}});

		public:
			HTTPStatus() noexcept = default;
			HTTPStatus(HTTPStatusCode value) noexcept
				:IndexedEnumValue(value)
			{
			}
			HTTPStatus(TInt value) noexcept
				:HTTPStatus(static_cast<HTTPStatusCode>(value))
			{
			}

		public:
			// IErrorCode
			bool IsSuccess() const noexcept override
			{
				// [200, 300)
				return IsInRange(200, 299);
			}
			bool IsFail() const noexcept override
			{
				// [400, +]
				return ToInt() >= 400;
			}

			uint32_t GetValue() const noexcept override
			{
				return IndexedEnumValue::ToInt();
			}
			void SetValue(uint32_t value) noexcept override
			{
				IndexedEnumValue::SetValue(static_cast<HTTPStatusCode>(value));
			}

			String ToString() const override
			{
				return String::FromView(IndexedEnumValue::ToString());
			}
			String GetMessage(const Locale& locale = {}) const override
			{
				// TODO: Add (localized if possible) error messages.
				return {};
			}

			// HTTPStatus
			HTTPStatusCode GetCode() const noexcept
			{
				return IndexedEnumValue::GetValue();
			}
			void SetCode(HTTPStatusCode value) noexcept
			{
				IndexedEnumValue::SetValue(value);
			}

			bool IsInformation() const noexcept
			{
				// [100, 200)
				return IsInRange(100, 199);
			}
			bool IsRedirection() const noexcept
			{
				// [300, 400)
				return IsInRange(300, 399);
			}
			bool IsClientError() const noexcept
			{
				// [400, 500)
				return IsInRange(400, 499);
			}
			bool IsServerError() const noexcept
			{
				// [500, 600)
				return IsInRange(500, 599);
			}

		public:
			HTTPStatus& AddFlag(TEnum value) noexcept = delete;
			HTTPStatus& RemoveFlag(TEnum value) noexcept = delete;
			HTTPStatus& ModFlag(TEnum value, bool condition) noexcept = delete;
			bool HasFlag(TEnum value) const noexcept = delete;
			bool HasSpecifiedFlagOnly(TEnum value) const noexcept = delete;

			explicit operator bool() const noexcept
			{
				return IsSuccess();
			}
			bool operator!() const noexcept
			{
				return IsFail();
			}
	};
}
