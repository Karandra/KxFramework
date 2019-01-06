/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxIndexedEnum.h"

enum class KxHTTPStatusCode: int32_t
{
	INVALID_CODE = -1,

	Continue = 100,
	SwitchingProtocols = 101,
	Processing = 102,
	EarlyHints = 103,

	OK = 200,
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

class KxHTTPStatusCodeDefinition: public KxIndexedEnum::Definition<KxHTTPStatusCodeDefinition, KxHTTPStatusCode, std::basic_string_view<wxChar, std::char_traits<wxChar>>, true>
{
	inline static constexpr const TItem ms_Index[] =
	{
		{KxHTTPStatusCode::Continue, wxS("Continue")},
		{KxHTTPStatusCode::SwitchingProtocols, wxS("SwitchingProtocols")},
		{KxHTTPStatusCode::Processing, wxS("Processing")},
		{KxHTTPStatusCode::EarlyHints, wxS("EarlyHints")},

		{KxHTTPStatusCode::OK, wxS("OK")},
		{KxHTTPStatusCode::Created, wxS("Created")},
		{KxHTTPStatusCode::Accepted, wxS("Accepted")},
		{KxHTTPStatusCode::NonAuthoritativeInformation, wxS("NonAuthoritativeInformation")},
		{KxHTTPStatusCode::NoContent, wxS("NoContent")},
		{KxHTTPStatusCode::ResetContent, wxS("ResetContent")},
		{KxHTTPStatusCode::PartialContent, wxS("PartialContent")},
		{KxHTTPStatusCode::MultiStatus, wxS("MultiStatus")},
		{KxHTTPStatusCode::AlreadyReported, wxS("AlreadyReported")},
		{KxHTTPStatusCode::IMUsed, wxS("IMUsed")},

		{KxHTTPStatusCode::MultipleChoices, wxS("MultipleChoices")},
		{KxHTTPStatusCode::MovedPermanently, wxS("MovedPermanently")},
		{KxHTTPStatusCode::Found, wxS("Found")},
		{KxHTTPStatusCode::SeeOther, wxS("SeeOther")},
		{KxHTTPStatusCode::NotModified, wxS("NotModified")},
		{KxHTTPStatusCode::UseProxy, wxS("UseProxy")},
		{KxHTTPStatusCode::SwitchProxy, wxS("SwitchProxy")},
		{KxHTTPStatusCode::TemporaryRedirect, wxS("TemporaryRedirect")},
		{KxHTTPStatusCode::PermanentRedirect, wxS("PermanentRedirect")},

		{KxHTTPStatusCode::BadRequest, wxS("BadRequest")},
		{KxHTTPStatusCode::Unauthorized, wxS("Unauthorized")},
		{KxHTTPStatusCode::PaymentRequired, wxS("PaymentRequired")},
		{KxHTTPStatusCode::Forbidden, wxS("Forbidden")},
		{KxHTTPStatusCode::NotFound, wxS("NotFound")},
		{KxHTTPStatusCode::MethodNotAllowed, wxS("MethodNotAllowed")},
		{KxHTTPStatusCode::NotAcceptable, wxS("NotAcceptable")},
		{KxHTTPStatusCode::ProxyAuthenticationRequired, wxS("ProxyAuthenticationRequired")},
		{KxHTTPStatusCode::RequestTimeout, wxS("RequestTimeout")},
		{KxHTTPStatusCode::Conflict, wxS("Conflict")},
		{KxHTTPStatusCode::Gone, wxS("Gone")},
		{KxHTTPStatusCode::LengthRequired, wxS("LengthRequired")},
		{KxHTTPStatusCode::PreconditionFailed, wxS("PreconditionFailed")},
		{KxHTTPStatusCode::PayloadTooLarge, wxS("PayloadTooLarge")},
		{KxHTTPStatusCode::URITooLong, wxS("URITooLong")},
		{KxHTTPStatusCode::UnsupportedMediaType, wxS("UnsupportedMediaType")},
		{KxHTTPStatusCode::RangeNotSatisfiable, wxS("RangeNotSatisfiable")},
		{KxHTTPStatusCode::ExpectationFailed, wxS("ExpectationFailed")},
		{KxHTTPStatusCode::ImATeapot, wxS("ImATeapot")},
		{KxHTTPStatusCode::UnprocessableEntity, wxS("UnprocessableEntity")},
		{KxHTTPStatusCode::Locked, wxS("Locked")},
		{KxHTTPStatusCode::FailedDependency, wxS("FailedDependency")},
		{KxHTTPStatusCode::UpgradeRequired, wxS("UpgradeRequired")},
		{KxHTTPStatusCode::PreconditionRequired, wxS("PreconditionRequired")},
		{KxHTTPStatusCode::TooManyRequests, wxS("TooManyRequests")},
		{KxHTTPStatusCode::RequestHeaderFieldsTooLarge, wxS("RequestHeaderFieldsTooLarge")},
		{KxHTTPStatusCode::UnavailableForLegalReasons, wxS("UnavailableForLegalReasons")},

		{KxHTTPStatusCode::InternalServerError, wxS("InternalServerError")},
		{KxHTTPStatusCode::NotImplemented, wxS("NotImplemented")},
		{KxHTTPStatusCode::BadGateway, wxS("BadGateway")},
		{KxHTTPStatusCode::ServiceUnavailable, wxS("ServiceUnavailable")},
		{KxHTTPStatusCode::GatewayTimeout, wxS("GatewayTimeout")},
		{KxHTTPStatusCode::HTTPVersionNotSupported, wxS("HTTPVersionNotSupported")},
		{KxHTTPStatusCode::VariantAlsoNegotiates, wxS("VariantAlsoNegotiates")},
		{KxHTTPStatusCode::InsufficientStorage, wxS("InsufficientStorage")},
		{KxHTTPStatusCode::LoopDetected, wxS("LoopDetected")},
		{KxHTTPStatusCode::NotExtended, wxS("NotExtended")},
		{KxHTTPStatusCode::NetworkAuthenticationRequired, wxS("NetworkAuthenticationRequired")},
	};
};

class KxHTTPStatusValue: public KxIndexedEnum::Value<KxHTTPStatusCodeDefinition, KxHTTPStatusCode::INVALID_CODE>
{
	public:
		KxHTTPStatusValue() = default;
		KxHTTPStatusValue(KxHTTPStatusCode value)
			:Value(value)
		{
		}
		KxHTTPStatusValue(TInt value)
			:KxHTTPStatusValue(static_cast<KxHTTPStatusCode>(value))
		{
		}

	public:
		void AddFlag(TEnum value) = delete;
		void RemoveFlag(TEnum value) = delete;
		bool HasFlag(TEnum value) const = delete;
		bool HasAnyFlag(TEnum valueCombo) const = delete;
};
