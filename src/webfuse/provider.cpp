#include "webfuse/provider.hpp"
#include "webfuse/ws/client.hpp"

#include <unistd.h>
#include <sys/stat.h>

#include <iostream>

namespace webfuse
{

class provider::detail
{
public:
    detail(filesystem_i & fs)
    : fs_(fs)
    , client([this](auto& reader) { return this->on_message(reader); })
    {

    }

    ~detail()
    {

    }

    void connect(std::string const & url)
    {
        client.connect(url);
    }

    void service()
    {
        client.service();
    }

    void set_connection_listener(std::function<void(bool)> listener)
    {
        client.set_connection_listener(listener);
    }

    messagewriter on_message(messagereader & reader)
    {
        auto const message_id = reader.read_u32();
        auto const req_type = get_request_type(reader.read_u8());
        auto const resp_type = get_response_type(req_type);

        messagewriter writer(resp_type);
        writer.set_id(message_id);

        switch (req_type)
        {
            case request_type::access:
                fs_access(reader, writer);
                break;
            case request_type::getattr:
                fs_getattr(reader, writer);
                break;
            case request_type::readdir:
                fs_readdir(reader, writer);
            default:
                std::cout << "unknown request: " << ((int) req_type) << std::endl;
                break;
        }

        return std::move(writer);
    }
private:
    void fs_access(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const mode = reader.read_access_mode();

        auto const result = fs_.access(path, mode);
        writer.write_i32(result);
    }

    void fs_getattr(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();

        struct stat buffer;
        auto const result = fs_.getattr(path, &buffer);
        writer.write_i32(result);
        if (0 == result)
        {
            writer.write_attr(&buffer);
        }
    }

    void fs_readdir(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        std::vector<std::string> entries;

        auto const result = fs_.readdir(path, entries, static_cast<uint64_t>(-1));
        writer.write_i32(result);
        if (0 == result)
        {
            writer.write_strings(entries);
        }
    }

    filesystem_i & fs_;
    ws_client client;
};

provider::provider(filesystem_i & fs)
: d(new detail(fs))
{

}

provider::~provider()
{
    delete d;
}

provider::provider(provider && other)
{
    this->d = other.d;
    other.d = nullptr;
}

provider& provider::operator=(provider && other)
{
    if (this != &other)
    {
        delete this->d;
        this->d = other.d;
        other.d = nullptr;
    }

    return *this;
}

void provider::connect(std::string const & url)
{
    d->connect(url);
}

void provider::service()
{
    d->service();
}

void provider::set_connection_listener(std::function<void(bool)> listener)
{
    d->set_connection_listener(listener);
}

}