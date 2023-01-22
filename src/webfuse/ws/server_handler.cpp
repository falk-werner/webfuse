#include "webfuse/ws/server_handler.hpp"
#include "webfuse/util/authenticator.hpp"

#include <iostream>

namespace
{
std::string get_auth_token_of_known_header(lws * wsi, lws_token_indexes header)
{
    std::string token;
    int const length = lws_hdr_total_length(wsi, header);
    if (length > 0)
    {
        std::vector<char> data(length + 1);
        int const actual_length = lws_hdr_copy(wsi, data.data(), length + 1, header);
        if (actual_length > 0)
        {
            token = data.data();
        }
    }        

    return token;
}

std::string get_auth_token_from_custom_header(lws * wsi, std::string const & auth_header)
{
    std::string token;
    int const length = lws_hdr_custom_length(wsi, auth_header.c_str(), auth_header.size());
    if (length > 0)
    {
        std::vector<char> data(length + 1);
        int const actual_length = lws_hdr_custom_copy(wsi, data.data(), length + 1, 
            auth_header.c_str(), auth_header.size());
        if (actual_length > 0)
        {
            token = data.data();
        }
    }        

    return token;
}

}

namespace webfuse
{

server_handler::server_handler(std::string const & auth_app, std::string const & auth_hdr)
: connection(nullptr)
, id(0)
, authenticator(auth_app)
, auth_header(auth_hdr)
{

}


int server_handler::filter_connection(lws * wsi)
{
    return authenticate_via_header(wsi);
}

int server_handler::on_established(lws * wsi)
{
    if (nullptr == connection)
    {
        connection = wsi;
        return 0;
    }

    return -1;
}


void server_handler::on_receive(lws * wsi, void* in, int len)
{
    auto * fragment = reinterpret_cast<char*>(in);
    current_message.append(fragment, len);
    if (0 != lws_is_final_fragment(wsi))
    {
        try
        {
            webfuse::messagereader reader(current_message);
            uint32_t id = reader.read_u32();
            reader.read_u8(); // read message type: ToDo: use it

            std::lock_guard<std::mutex> lock(mut);
            auto it = pending_responses.find(id);
            if (it != pending_responses.end())
            {
                it->second.set_value(std::move(reader));
                pending_responses.erase(it);
            }
            else
            {
                // ToDo: log request not found
                std::cout << "warning: request not found: id=" << id << std::endl;
                for(auto const & entry: pending_responses)
                {
                    std::cout << "\t" << entry.first << std::endl;
                }
            }
        }
        catch(...)
        {
            // ToDo: log invalid message
            std::cout << "warning: invalid message" << std::endl;
        }
    }
}

void server_handler::on_writable()
{
    webfuse::messagewriter writer(webfuse::request_type::unknown);
    bool has_msg = false;
    bool has_more = false;

    {
        std::lock_guard<std::mutex> lock(mut);
        has_msg = !(requests.empty());
        if (has_msg)
        {
            has_msg = true;
            writer = std::move(requests.front());
            requests.pop();

            has_more = !(requests.empty());
        }
    }

    if (has_msg)
    {
        size_t size;
        unsigned char * raw_data = writer.get_data(size);
        lws_write(connection, raw_data, size, LWS_WRITE_BINARY);
    }

    if (has_more)
    {
        lws_callback_on_writable(connection);
    }
}

void server_handler::on_closed(lws * wsi)
{
    if (wsi == connection)
    {
        connection = nullptr;
    }
}

void server_handler::poll()
{
    std::lock_guard<std::mutex> lock(mut);
    if (!requests.empty())
    {
        if (nullptr != connection)
        {
            lws_callback_on_writable(connection);
        }
        else
        {
            requests = std::move(std::queue<webfuse::messagewriter>());
            pending_responses.clear();
        }
    }
}


std::future<messagereader> server_handler::perform(messagewriter writer)
{
    std::future<messagereader> result;
    {
        std::promise<messagereader> p;
        result = p.get_future();

        std::lock_guard<std::mutex> lock(mut);
        uint32_t id = next_id();
        writer.set_id(id);
        requests.emplace(std::move(writer));
        pending_responses.emplace(id, std::move(p));
    }

    return result;

}


int server_handler::authenticate_via_header(lws * wsi)
{
    int result = 0;
    if ((!authenticator.empty()) && (!auth_header.empty()))
    {
        std::string token = get_auth_token(wsi);
        if (!token.empty())
        {
            webfuse::authenticator auth(authenticator);
            result = auth.authenticate(token) ? 0 : -1;
        }
        else
        {
            result = -1;
        }
    }

    return result;
}

std::string server_handler::get_auth_token(lws * wsi) const
{
    if (auth_header == "authorization")
    {
        return get_auth_token_of_known_header(wsi, WSI_TOKEN_HTTP_AUTHORIZATION);
    }

    if (auth_header == "x-auth-token")
    {
        return get_auth_token_of_known_header(wsi, WSI_TOKEN_X_AUTH_TOKEN);
    }

    return get_auth_token_from_custom_header(wsi, auth_header);
}

uint32_t server_handler::next_id()
{
    id++;
    if (0 == id)
    {
        id = 1;
    }
    return id;
}


}