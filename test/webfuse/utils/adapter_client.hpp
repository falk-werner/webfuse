#ifndef WF_UTILS_ADAPTER_CLIENT_HPP
#define WF_UTILS_APAPTER_CLIENT_HPP

#include "webfuse/adapter/client.h"
#include <string>

namespace webfuse_test
{

class AdapterClient
{
    AdapterClient(AdapterClient const &) = delete;
    AdapterClient& operator=(AdapterClient const &) = delete;
public:
    AdapterClient(
        wf_client_callback_fn * callback,
        void * user_data,
        std::string const & url);
    ~AdapterClient();
    void Connect();
    void Disconnect();
    void Authenticate();
    void AddFileSystem();
    std::string GetDir() const;
private:
    class Private;
    Private * d;
};

}

#endif
