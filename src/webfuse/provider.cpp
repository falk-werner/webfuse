#include "webfuse/provider.hpp"
#include "webfuse/ws/client.hpp"

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

    messagewriter on_message(messagereader & reader)
    {
        auto message_id = reader.read_u32();
        auto request_type = reader.read_u8();

        messagewriter writer(response_type::unknown);
        writer.set_id(message_id);

        switch (request_type)
        {

            default:
                break;
        }

        return std::move(writer);
    }
private:
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


}