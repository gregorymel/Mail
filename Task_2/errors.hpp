#pragma once

#include <system_error>

namespace xmpp_error
{
	enum class xmpp_error_t
	{
		ok                      = 0,
        redirect                = 302,
        bad_request             = 400,
        not_authorized          = 401,
        payment_required        = 402,
        forbidden               = 403,
        item_not_found          = 404,
        not_allowed             = 405,
        not_acceptable          = 406,
        registration_required   = 407,
        conflict                = 409,
        feature_not_implemented = 501,
        service_unavailable     = 503,
        internal_server_error   = 500,
        remote_server_timeout   = 504
	};


	class xmpp_category_impl : public std::error_category
	{
	public:
		virtual const char* name() const noexcept;
		virtual std::string message( int ev) const;
	};

	std::error_code make_error_code( xmpp_error_t e);
    std::error_condition make_error_condition( xmpp_error_t e);
}

const char* xmpp_error::xmpp_category_impl::name() const noexcept
{
		return "xmpp";
}

std::string xmpp_error::xmpp_category_impl::message( int ev) const
{
	xmpp_error_t err = static_cast<xmpp_error_t>(  ev);

    switch ( err )
    {
    	case xmpp_error_t::ok:
        	return "OK";

    	case xmpp_error_t::redirect:
        	return "Redirect";

    	case xmpp_error_t::bad_request:
        	return "Bad request";

    	case xmpp_error_t::not_authorized:
        	return "Not authorized";

    	case xmpp_error_t::payment_required:
        	return "Payment required";

    	case xmpp_error_t::forbidden:
        	return "Forbidden";

    	case xmpp_error_t::item_not_found:
        	return "Item not found";

    	case xmpp_error_t::not_allowed:
        	return "Not allowed";

    	case xmpp_error_t::not_acceptable:
        	return "Not acceptable";

    	case xmpp_error_t::registration_required:
        	return "Registration required";

    	case xmpp_error_t::conflict:
        	return "Conflict";

    	case xmpp_error_t::feature_not_implemented:
        	return "Feature not implemented";

    	case xmpp_error_t::service_unavailable:
        	return "Service unavailable";

    	case xmpp_error_t::internal_server_error:
        	return "Internal server error";

    	case xmpp_error_t::remote_server_timeout:
        	return "Remote server timeout";

    	default:
        	return "Unknown XMPP error";
    }				
}

const std::error_category& xmpp_category()
{
    static xmpp_error::xmpp_category_impl xmpp_category_instance;
    return xmpp_category_instance;
}

std::error_code xmpp_error::make_error_code( xmpp_error_t e)
{
	return std::error_code( static_cast<int>( e), xmpp_category());
}

std::error_condition xmpp_error::make_error_condition( xmpp_error_t e)
{
	return std::error_condition( static_cast<int>( e), xmpp_category());
}

namespace std
{
	template<>
	struct is_error_code_enum<xmpp_error::xmpp_error_t> : public true_type {};
}