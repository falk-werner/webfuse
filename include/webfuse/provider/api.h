////////////////////////////////////////////////////////////////////////////////
/// \file provider/api.h
/// \brief API define for webfuse provider.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_PROVIDER_API_H
#define WFP_PROVIDER_API_H

//------------------------------------------------------------------------------
/// \def WFP_API
/// \brief Marks public symbols of libwebfuse_provider.
//------------------------------------------------------------------------------
#ifndef WFP_API
#define WFP_API
#endif

//------------------------------------------------------------------------------
/// \def WFP_EXPORT
/// \brief Marks exported symbols as visible.
///
/// Set WFP_API to WFP_EXPORT when building libwebfuse_provider.so to export
/// public symbols.
//------------------------------------------------------------------------------
#ifndef WFP_EXPORT
#ifdef __GNUC__
#define WFP_EXPORT __attribute__ ((visibility ("default")))
#else
#define WFP_EXPORT
#endif
#endif

#endif
