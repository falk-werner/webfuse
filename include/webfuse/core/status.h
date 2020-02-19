////////////////////////////////////////////////////////////////////////////////
/// \file status.h
/// \brief Generic status code.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_STATUS_H
#define WF_STATUS_H

#define WF_GOOD 0   ///< Positive status code.
#define WF_BAD  1   ///< Generic negative status code.

#define WF_BAD_NOTIMPLEMENTED 2     ///< The called function is not implemented (yet).
#define WF_BAD_TIMEOUT        3     ///< A timeout occured.
#define WF_BAD_BUSY           4     ///< Resource is busy, try again later.
#define WF_BAD_FORMAT         5     ///< Invalid format.

#define WF_BAD_NOENTRY 101          ///< Entry not found.
#define WF_BAD_ACCESS_DENIED 102    ///< Access is denied.

/// Status code.
typedef int wf_status;

#endif
