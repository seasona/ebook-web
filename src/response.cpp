#include "response.h"
#include "spdlog/spdlog.h"
#include <unordered_map>

namespace Jhttp {

struct HttpStatusSummaryHelper {
    friend char const* HttpStatusSummaryGetter(int);

private:
    HttpStatusSummaryHelper() { init(); }

    void init();
    std::unordered_map<int, char const*> status_map;
};

#define ADD_SUMMARY(CODE, SUMMARY) status_map[CODE] = #SUMMARY

const char http_head[] = "HTTP/";
const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};

void HttpStatusSummaryHelper::init() {
    ADD_SUMMARY(HTTP_STATUS_CONTINUE, Continue);
    ADD_SUMMARY(HTTP_STATUS_SWITCHING_PROTOCOLS, Switching Protocols);
    ADD_SUMMARY(HTTP_STATUS_PROCESSING, Processing);
    ADD_SUMMARY(HTTP_STATUS_OK, OK);
    ADD_SUMMARY(HTTP_STATUS_CREATED, Created);
    ADD_SUMMARY(HTTP_STATUS_ACCEPTED, Accepted);
    ADD_SUMMARY(HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION,
                Non - Authoritative Information);
    ADD_SUMMARY(HTTP_STATUS_NO_CONTENT, No Content);
    ADD_SUMMARY(HTTP_STATUS_RESET_CONTENT, Reset Content);
    ADD_SUMMARY(HTTP_STATUS_PARTIAL_CONTENT, Partial Content);
    ADD_SUMMARY(HTTP_STATUS_MULTI_STATUS, Multi - Status);
    ADD_SUMMARY(HTTP_STATUS_ALREADY_REPORTED, Already Reported);
    ADD_SUMMARY(HTTP_STATUS_IM_USED, IM Used);
    ADD_SUMMARY(HTTP_STATUS_MULTIPLE_CHOICES, Multiple Choices);
    ADD_SUMMARY(HTTP_STATUS_MOVED_PERMANENTLY, Moved Permanently);
    ADD_SUMMARY(HTTP_STATUS_FOUND, Found);
    ADD_SUMMARY(HTTP_STATUS_SEE_OTHER, See Other);
    ADD_SUMMARY(HTTP_STATUS_NOT_MODIFIED, Not Modified);
    ADD_SUMMARY(HTTP_STATUS_USE_PROXY, Use Proxy);
    ADD_SUMMARY(HTTP_STATUS_TEMPORARY_REDIRECT, Temporary Redirect);
    ADD_SUMMARY(HTTP_STATUS_PERMANENT_REDIRECT, Permanent Redirect);
    ADD_SUMMARY(HTTP_STATUS_BAD_REQUEST, Bad Request);
    ADD_SUMMARY(HTTP_STATUS_UNAUTHORIZED, Unauthorized);
    ADD_SUMMARY(HTTP_STATUS_PAYMENT_REQUIRED, Payment Required);
    ADD_SUMMARY(HTTP_STATUS_FORBIDDEN, Forbidden);
    ADD_SUMMARY(HTTP_STATUS_NOT_FOUND, Not Found);
    ADD_SUMMARY(HTTP_STATUS_METHOD_NOT_ALLOWED, Method Not Allowed);
    ADD_SUMMARY(HTTP_STATUS_NOT_ACCEPTABLE, Not Acceptable);
    ADD_SUMMARY(HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED,
                Proxy Authentication Required);
    ADD_SUMMARY(HTTP_STATUS_REQUEST_TIMEOUT, Request Timeout);
    ADD_SUMMARY(HTTP_STATUS_CONFLICT, Conflict);
    ADD_SUMMARY(HTTP_STATUS_GONE, Gone);
    ADD_SUMMARY(HTTP_STATUS_LENGTH_REQUIRED, Length Required);
    ADD_SUMMARY(HTTP_STATUS_PRECONDITION_FAILED, Precondition Failed);
    ADD_SUMMARY(HTTP_STATUS_PAYLOAD_TOO_LARGE, Payload Too Large);
    ADD_SUMMARY(HTTP_STATUS_URI_TOO_LONG, URI Too Long);
    ADD_SUMMARY(HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE, Unsupported Media Type);
    ADD_SUMMARY(HTTP_STATUS_RANGE_NOT_SATISFIABLE, Range Not Satisfiable);
    ADD_SUMMARY(HTTP_STATUS_EXPECTATION_FAILED, Expectation Failed);
    ADD_SUMMARY(HTTP_STATUS_MISDIRECTED_REQUEST, Misdirected Request);
    ADD_SUMMARY(HTTP_STATUS_UNPROCESSABLE_ENTITY, Unprocessable Entity);
    ADD_SUMMARY(HTTP_STATUS_LOCKED, Locked);
    ADD_SUMMARY(HTTP_STATUS_FAILED_DEPENDENCY, Failed Dependency);
    ADD_SUMMARY(HTTP_STATUS_UPGRADE_REQUIRED, Upgrade Required);
    ADD_SUMMARY(HTTP_STATUS_PRECONDITION_REQUIRED, Precondition Required);
    ADD_SUMMARY(HTTP_STATUS_TOO_MANY_REQUESTS, Too Many Requests);
    ADD_SUMMARY(HTTP_STATUS_REQUEST_HEADER_FIELDS_TOO_LARGE,
                Request Header Fields Too Large);
    ADD_SUMMARY(HTTP_STATUS_UNAVAILABLE_FOR_LEGAL_REASONS,
                Unavailable For Legal Reasons);
    ADD_SUMMARY(HTTP_STATUS_INTERNAL_SERVER_ERROR, Internal Server Error);
    ADD_SUMMARY(HTTP_STATUS_NOT_IMPLEMENTED, Not Implemented);
    ADD_SUMMARY(HTTP_STATUS_BAD_GATEWAY, Bad Gateway);
    ADD_SUMMARY(HTTP_STATUS_SERVICE_UNAVAILABLE, Service Unavailable);
    ADD_SUMMARY(HTTP_STATUS_GATEWAY_TIMEOUT, Gateway Timeout);
    ADD_SUMMARY(HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED,
                HTTP Version Not Supported);
    ADD_SUMMARY(HTTP_STATUS_VARIANT_ALSO_NEGOTIATES, Variant Also Negotiates);
    ADD_SUMMARY(HTTP_STATUS_INSUFFICIENT_STORAGE, Insufficient Storage);
    ADD_SUMMARY(HTTP_STATUS_LOOP_DETECTED, Loop Detected);
    ADD_SUMMARY(HTTP_STATUS_NOT_EXTENDED, Not Extended);
    ADD_SUMMARY(HTTP_STATUS_NETWORK_AUTHENTICATION_REQUIRED,
                Network Authentication Required);
}

char const* HttpStatusSummaryGetter(int status) {
    static HttpStatusSummaryHelper status_summary;
    auto it = status_summary.status_map.find(status);
    return (it != status_summary.status_map.end()) ? it->second : "";
}

std::vector<asio::const_buffer> Response::ToBuffers() {
    std::vector<asio::const_buffer> write_buffer;
    //! the data in asio::buffer must be exist until async_write, so the
    //! response_status must be static
    static std::string response_status =
        "HTTP/" + std::to_string(version_major) + "." +
        std::to_string(version_minor) + " " + std::to_string(status) + " " +
        HttpStatusSummaryGetter(status) + "\r\n";
    write_buffer.push_back(asio::buffer(response_status));
    for (std::size_t i = 0; i < headers.size(); ++i) {
        auto& h = headers[i];
        write_buffer.push_back(asio::buffer(h.first));
        write_buffer.push_back(asio::buffer(name_value_separator));
        write_buffer.push_back(asio::buffer(h.second));
        write_buffer.push_back(asio::buffer(crlf));
    }
    write_buffer.push_back(asio::buffer(crlf));
    write_buffer.push_back(asio::buffer(content));

    return write_buffer;
}

std::ostream& operator<<(std::ostream& os, const Response& rep) {
    os << "HTTP/" << rep.version_major << "." << rep.version_minor << " "
       << rep.status << " " << HttpStatusSummaryGetter(rep.status) << "\r\n";

    for (auto& x : rep.headers) {
        os << x.first << ":" << x.second << "\r\n";
    }

    os << "\r\n" << rep.content;
}

}  // namespace Jhttp