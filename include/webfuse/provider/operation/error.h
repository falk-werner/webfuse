////////////////////////////////////////////////////////////////////////////////
/// \file provider/operation/error.h
/// \brief Respond with error code.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_OPERATION_ERROR_H
#define WFP_OPERATION_ERROR_H

#include "webfuse/provider/api.h"
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_request;

//------------------------------------------------------------------------------
/// \brief Respond to a request with an error.
/// 
/// A client's callback must respond with exactly one responde, either with a
/// valid reponse regarding to the concrete request or with an error response.
///
/// \param request pointer to request
/// \param status error code
//------------------------------------------------------------------------------
extern WFP_API void wfp_respond_error(
    struct wfp_request * request,
    wf_status status);

#ifdef __cplusplus
}
#endif

#endif
