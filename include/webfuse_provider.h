////////////////////////////////////////////////////////////////////////////////
/// \file webfuse_provider.h
/// \brief Convenience header to include all functionality of libfuse_provider.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_PROVIDER_H
#define WF_PROVIDER_H

#include <webfuse/core/status.h>
#include <webfuse/core/protocol_names.h>

#include <webfuse/provider/api.h>
#include <webfuse/provider/client.h>
#include <webfuse/provider/client_config.h>
#include <webfuse/provider/client_protocol.h>
#include <webfuse/provider/dirbuffer.h>
#include <webfuse/provider/credentials.h>

#include <webfuse/provider/operation/error.h>
#include <webfuse/provider/operation/lookup.h>
#include <webfuse/provider/operation/getattr.h>
#include <webfuse/provider/operation/readdir.h>
#include <webfuse/provider/operation/open.h>
#include <webfuse/provider/operation/close.h>
#include <webfuse/provider/operation/read.h>

#endif
