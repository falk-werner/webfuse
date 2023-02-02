#include <b64/decode.h>
#include <security/pam_appl.h>
#include <syslog.h>

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <string>
#include <sstream>

namespace
{

bool decode(std::string const & token, std::string & username, std::string & password)
{
    std::istringstream encoded(token);
    std::stringstream decoded;

    base64::decoder decoder;
    decoder.decode(encoded, decoded);
    auto const plain = decoded.str();
    auto const pos = plain.find(':');
    bool const success = (pos != std::string::npos);
    if (success)
    {
        username = plain.substr(0, pos);
        password = plain.substr(pos + 1);
    }

    return success;
}

struct credentials
{
    std::string username;
    std::string password;
};

extern "C" int conversation(
    int count,
    struct pam_message const ** messages,
    struct pam_response ** ret_responses,
    void * user_data)
{
    if (count <= 0) { return PAM_CONV_ERR; }

    int result = PAM_SUCCESS;
    auto * responses = reinterpret_cast<struct pam_response *>(malloc(count * sizeof(struct pam_response)));
    auto * creds = reinterpret_cast<credentials*>(user_data);

    for(int i = 0; (PAM_SUCCESS == result) && (i < count); i++)
    {
        auto * response = &responses[i];
        auto const * message = messages[i];

        response->resp_retcode = 0;
        response->resp = nullptr;

        switch(message->msg_style)
        {
            case PAM_PROMPT_ECHO_ON:
                response->resp = strdup(creds->username.c_str());
                break;
            case PAM_PROMPT_ECHO_OFF:
                response->resp = strdup(creds->password.c_str());
                break;
            case PAM_TEXT_INFO:
                break;
            case PAM_ERROR_MSG:
                break;
            default:
                free(responses);
                result = PAM_CONV_ERR;
                break;
        }
    }

    if (PAM_SUCCESS == result)
    {
        *ret_responses = responses;
    }

    return result;
}

bool authenticate(std::string const & username, std::string const & password)
{
    credentials creds;
    creds.username = username;
    creds.password = password;

    struct pam_conv conv;
    conv.conv = &conversation;
    conv.appdata_ptr = reinterpret_cast<void*>(&creds);

    pam_handle_t * handle = nullptr;
    bool cleanup_handle = false;
    bool result = true;
    {
        auto const rc = pam_start("webfuse", nullptr, &conv, &handle);
        result = (PAM_SUCCESS == rc);
        cleanup_handle = result;
        if (!result)
        {
            syslog(LOG_AUTH, "failed to start PAM conversation");
        }
    }

    if (result)
    {
        pam_set_item(handle, PAM_USER, reinterpret_cast<void const*>(username.c_str()));
        auto const rc = pam_authenticate(handle, PAM_DISALLOW_NULL_AUTHTOK);
        result = (PAM_SUCCESS == rc);
    }

    if (result)
    {
        auto const rc = pam_acct_mgmt(handle, PAM_DISALLOW_NULL_AUTHTOK);
        result = (PAM_SUCCESS == rc);
    }

    if (cleanup_handle)
    {
        pam_end(handle, 0);
    }

    return result;
}

}

int main(int argc, char* argv[])
{
    int exit_code = EXIT_FAILURE;
    bool print_usage = true;

    if (argc == 2)
    {
        std::string const token = argv[1];
        if (("-h" != token) && ("--help" != token))
        {
            print_usage = false;

            openlog("webfuse_pam_auth", 0, LOG_AUTH);

            std::string username;
            std::string password;
            auto const decode_valid = decode(token, username, password);
            if (decode_valid)
            {
                auto const is_authenticated = authenticate(username, password);
                if (is_authenticated)
                {
                    syslog(LOG_AUTH, "authenticate user \"%s\"", username.c_str());
                    exit_code = EXIT_SUCCESS;
                }
                else
                {
                    syslog(LOG_AUTH, "failed to authenticate user \"%s\"", username.c_str());
                }
            }
            else
            {
                syslog(LOG_AUTH, "failed to decode authentication token");
            }

            closelog();
        }
    }


    if (print_usage)
    {
        std::cout << R"(webfuse_pam_authenticator, (c) 2023 Falk Werner
webfuse PAM authenticator

Usage:
    webfuse_pam_authenticator <token>

Arguments:
    <token> token used for authentication
            token := base64(<username> ":" <password>)
)";
    }

    return exit_code;
}