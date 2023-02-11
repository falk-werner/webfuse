#include <b64/decode.h>

#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char* argv[])
{
    int exit_code = EXIT_FAILURE;

    if (argc == 2)
    {
        std::istringstream token(argv[1]);

        base64::decoder decoder;
        std::stringstream plain;
        decoder.decode(token, plain);

        auto const plain_str = plain.str();
        auto const pos = plain_str.find(':');
        if (pos != std::string::npos)
        {
            auto const username = plain_str.substr(0, pos);
            auto const password = plain_str.substr(pos + 1);

            std::cout << "username: " << username << std::endl;
            std::cout << "password: " << password << std::endl;
            exit_code = EXIT_SUCCESS;
        }

        if (exit_code != EXIT_SUCCESS)
        {
            std::cerr << "error: failed to decode" << std::endl;
        }
    }
    else
    {
        std::cout << R"(webfuse_pam_token_encode, (c) 2023 Falk Werner
    Encode token for webfuse PAM authenticator

    Usage:
        webfuse_pam_token_encode <username> <password>
)";
    }

    return exit_code;
}