#ifndef WF_JSON_MATCHER_HPP
#define FW_JSON_MATCHER_HPP

#include <gtest/gtest.h>
#include <jansson.h>

namespace webfuse_test
{

MATCHER_P(JsonMatcher, expected_str, "") 
{
    std::cout << "--- JsonMatcher ---" << std::endl;
    bool matches = false;
    json_t * expected = json_loads(expected_str, 0, nullptr); 
    if (nullptr != expected)
    {
        matches = (1 == json_equal(expected, arg)); 
        if (!matches)
        {
            char * actual = json_dumps(arg, 0);
            std::cout << actual << std::endl;
            *result_listener << "where arg is " << actual;
            free(actual);
        }

        json_decref(expected);
    }

    return true; //matches; 
}

}

#endif
