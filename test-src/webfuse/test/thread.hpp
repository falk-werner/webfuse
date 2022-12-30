#ifndef WEBFUSE_THREAD_HPP
#define WEBFUSE_THREAD_HPP

#include <pthread.h>
#include <functional>

namespace webfuse
{

class thread
{
public:
    explicit thread(std::function<void(void)> run);
    ~thread();
    void kill(int signal_id);
private:
    pthread_t real_thread;
};

}

#endif
