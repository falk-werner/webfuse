#include "webfuse/test/thread.hpp"
#include <csignal>

namespace
{

extern "C" void * webfuse_thread_main(void * args)
{
    auto * run = reinterpret_cast<std::function<void(void)> *>(args);
    (*run)();
    return nullptr;
}

}


namespace webfuse
{

thread::thread(std::function<void(void)> run)
{
    pthread_create(&real_thread, nullptr, 
        &webfuse_thread_main,
        reinterpret_cast<void*>(&run));

}

thread::~thread()
{
    pthread_join(real_thread, nullptr);
}

void thread::kill(int signal_id)
{
    pthread_kill(real_thread, signal_id);
}


}