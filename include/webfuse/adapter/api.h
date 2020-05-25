////////////////////////////////////////////////////////////////////////////////
/// \file adapter/api.h
/// \brief API define for webfuse adapter.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_API_H
#define WF_ADAPTER_API_H

//------------------------------------------------------------------------------
/// \def WF_API
/// \brief Marks public symbols of libwebfuse_adapter.
//------------------------------------------------------------------------------
#ifndef WF_API
#define WF_API
#endif

//------------------------------------------------------------------------------
/// \def WF_EXPORT
/// \brief Marks exported symbols as visible.
///
/// Set WF_API to WF_EXPORT when building libwebfuse_adapter.so to export
/// public symbols.
//------------------------------------------------------------------------------
#ifndef WF_EXPORT
#ifdef __GNUC__
#define WF_EXPORT __attribute__ ((visibility ("default")))
#else
#define WF_EXPORT
#endif
#endif

#endif
