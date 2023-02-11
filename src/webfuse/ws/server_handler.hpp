#ifndef WEBFUSE_SERVER_HANDLER_HPP
#define WEBFUSE_SERVER_HANDLER_HPP

#include "webfuse/ws/messagereader.hpp"
#include "webfuse/ws/messagewriter.hpp"

#include <libwebsockets.h>

#include <string>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <future>
#include <atomic>

namespace webfuse
{

class server_handler
{
public:
    server_handler(std::string const & auth_app, std::string const & auth_hdr);

    int filter_connection(lws * wsi);

    int  on_established(lws* wsi);
    void on_receive(lws * wsi, void* in, int len);
    int on_writable();
    void on_closed(lws * wsi);

    std::future<messagereader> perform(messagewriter writer);
    void poll();

private:
    int authenticate_via_header(lws * wsi);
    std::string get_auth_token(lws * wsi) const;
    uint32_t next_id();
    void finish_authentication(lws * wsi, messagereader reader);

    struct lws * connection;
    uint32_t id;
    bool shutdown_requested;

    std::atomic<bool> is_authenticated;
    std::string authenticator;
    std::string auth_header;

    std::string current_message;

    std::mutex mut;
    std::queue<webfuse::messagewriter> requests;
    std::unordered_map<uint32_t, std::promise<webfuse::messagereader>> pending_responses;



};


}

#endif
