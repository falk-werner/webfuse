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

    void interrupt()
    {
        client.interrupt();
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
            case request_type::readlink:
                fs_readlink(reader, writer);
                break;
            case request_type::symlink:
                fs_symlink(reader, writer);
                break;
            case request_type::link:
                fs_link(reader, writer);
                break;
            case request_type::rename:
                fs_rename(reader, writer);
                break;
            case request_type::chmod:
                fs_chmod(reader, writer);
                break;
            case request_type::chown:
                fs_chown(reader, writer);
                break;
            case request_type::truncate:
                fs_truncate(reader, writer);
                break;
            case request_type::fsync:
                fs_fsync(reader, writer);
                break;
            case request_type::utimens:
                fs_utimens(reader, writer);
                break;
            case request_type::open:
                fs_open(reader, writer);
                break;
            case request_type::mknod:
                fs_mknod(reader, writer);
                break;
            case request_type::create:
                fs_create(reader, writer);
                break;
            case request_type::release:
                fs_release(reader, writer);
                break;
            case request_type::unlink:
                fs_unlink(reader, writer);
                break;
            case request_type::read:
                fs_read(reader, writer);
                break;
            case request_type::write:
                fs_write(reader, writer);
                break;
            case request_type::readdir:
                fs_readdir(reader, writer);
                break;
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

    void fs_readlink(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        std::string out;

        auto const result = fs_.readlink(path, out);
        writer.write_i32(result);
        if (0 == result)
        {
            writer.write_str(out);
        }
    }

    void fs_symlink(messagereader & reader, messagewriter & writer)
    {
        auto const from = reader.read_str();
        auto const to = reader.read_str();

        auto const result = fs_.symlink(from, to);
        writer.write_i32(result);
    }

    void fs_link(messagereader & reader, messagewriter & writer)
    {
        auto const from = reader.read_str();
        auto const to = reader.read_str();

        auto const result = fs_.link(from, to);
        writer.write_i32(result);
    }

    void fs_rename(messagereader & reader, messagewriter & writer)
    {
        auto const from = reader.read_str();
        auto const to = reader.read_str();
        auto const flags = reader.read_u8();

        auto const result = fs_.rename(from, to, flags);
        writer.write_i32(result);
    }

    void fs_chmod(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const mode = reader.read_mode();

        auto const result = fs_.chmod(path, mode);
        writer.write_i32(result);
    }

    void fs_chown(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const uid = static_cast<uid_t>(reader.read_u32());
        auto const gid = static_cast<gid_t>(reader.read_u32());

        auto const result = fs_.chown(path, uid, gid);
        writer.write_i32(result);
    }

    void fs_truncate(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const size = reader.read_u64();
        auto const handle = reader.read_u64();

        auto const result = fs_.truncate(path, size, handle);
        writer.write_i32(result);
    }

    void fs_fsync(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const is_datasync = reader.read_bool();
        auto const handle = reader.read_u64();

        auto const result = fs_.fsync(path, is_datasync, handle);
        writer.write_i32(result);
    }

    void fs_utimens(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        struct timespec times[2];
        reader.read_time(times[0]);
        reader.read_time(times[1]);
        auto const handle = reader.read_u64();

        auto const result = fs_.utimens(path, times, handle);
        writer.write_i32(result);
    }

    void fs_open(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const flags = reader.read_openflags();
        uint64_t handle = static_cast<uint64_t>(-1);

        auto const result = fs_.open(path, flags, handle);
        writer.write_i32(result);
        if (result == 0)
        {
            writer.write_u64(handle);
        } 
    }

    void fs_mknod(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const mode = reader.read_mode();
        auto const dev = reader.read_u32();

        auto const result = fs_.mknod(path, mode, dev);
        writer.write_i32(result);
    }

    void fs_create(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const mode = reader.read_mode();
        uint64_t handle = static_cast<uint64_t>(-1);

        auto const result = fs_.create(path, mode, handle);
        writer.write_i32(result);
        if (result == 0)
        {
            writer.write_u64(handle);
        }
    }

    void fs_release(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const handle = reader.read_u64();

        auto const result = fs_.release(path, handle);
        writer.write_i32(result);
    }

    void fs_unlink(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();

        auto const result = fs_.unlink(path);
        writer.write_i32(result);
    }

    void fs_read(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const size = reader.read_u32();
        auto const offset = reader.read_u64();
        auto const handle = reader.read_u64();

        std::vector<char> buffer(size);

        auto const result = fs_.read(path, buffer.data(), size, offset, handle);        
        writer.write_i32(result);
        if (0 < result)
        {
            writer.write_data(buffer.data(), result);
        }
    }

    void fs_write(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        auto const buffer = reader.read_bytes();
        auto const offset = reader.read_u64();
        auto const handle = reader.read_u64();

        auto const result = fs_.write(path, buffer.c_str(), buffer.size(), offset, handle);        
        writer.write_i32(result);
    }

    void fs_readdir(messagereader & reader, messagewriter & writer)
    {
        auto const path = reader.read_str();
        std::vector<std::string> entries;

        auto const result = fs_.readdir(path, entries);
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

void provider::interrupt()
{
    d->interrupt();
}

void provider::set_connection_listener(std::function<void(bool)> listener)
{
    d->set_connection_listener(listener);
}

}