#include "webfuse/webfuse.hpp"
#include "webfuse/test/thread.hpp"
#include "webfuse/test/tempdir.hpp"
#include <gtest/gtest.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <thread>

extern "C" void * run(void * args)
{
    webfuse::app * app = reinterpret_cast<webfuse::app*>(args);

    return nullptr;
}

TEST(app, init)
{
    webfuse::app app;
}

TEST(app, run)
{   
    webfuse::tempdir dir;
    webfuse::thread thread([&dir](){
        webfuse::app app;
        char arg0[] = "webfuse";
        char arg1[] = "-f";
        char* arg2 = strdup(dir.name().c_str());
        char* argv[] = { arg0, arg1, arg2, nullptr};
        int rc = app.run(3, argv);
        free(arg2);
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    thread.kill(SIGINT);
}