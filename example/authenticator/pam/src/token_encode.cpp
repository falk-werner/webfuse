#include <b64/encode.h>

#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char* argv[])
{
    if (argc == 3)
    {
        std::string const username = argv[1];
        std::string const password = argv[2];

        base64::encoder encoder;
        std::istringstream plain(username + ':' + password);
        std::stringstream token;
        encoder.encode(plain, token);

        std::cout << token.str();
    }
    else
    {
        std::cout << R"(webfuse_pam_token_encode, (c) 2023 Falk Werner
    Encode token for webfuse PAM authenticator

    Usage:
        webfuse_pam_token_encode <username> <password>
)";
    }

    return EXIT_SUCCESS;
}