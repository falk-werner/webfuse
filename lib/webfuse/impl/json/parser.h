#ifndef WF_IMPL_JSON_PARSER_H
#define WF_IMPL_JSON_PARSER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_json_reader;
struct wf_json;

extern bool
wf_impl_json_parse_value(
    struct wf_json_reader * reader,
    struct wf_json * json);

#ifdef __cplusplus
}
#endif

#endif
