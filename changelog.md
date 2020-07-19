# webfuse changelog

## 0.5.0 _(Sun Jul 19 2020)_

*   *Feature*: Remove dependency to libjansson
*   *Feature*: Increase performace
*   *Fix*: Fix deadlock reading files larger that a few KBytes (Isseue #82)

## 0.4.0 _(Sun Jul 05 2020)_

*   *Breaking Change*: Remove CMake support (change build system to meson)
*   *Breaking Change*: Make argument credentials const in `wf_authenticate_fn`
*   *Breaking Change*: Moved provider library into [separate project](https://github.com/falk-werner/webfuse-provider)
*   *Breaking Change*:Renamed library (libwebfuse_adapter -> libwebfuse)
*   *Feature*: Add adapter client implementation
*   *Feature*: Allow system to choose port of webfuse server (by setting port in `wf_server_config` to 0)
*   *Fix*: Fix meson build failure when gtest or jansson are not installed
*   *Fix*: Fix crash if libfuse is not available at runtime

## 0.3.0 _(Fri Jun 05 2020)_

*   *Breaking Change*: Remove argument `timeout_ms` in  `wf_server_service` and `wfp_client_service`
*   *Breaking Change*: Remove `wfp_client_ontimer`
*   *Feature*: Add meson build support
*   *Feature*: Add `wf_server_interrupt` and `wfp_client_interrupt`

## 0.2.0 _(Sun Mar 01 2020)_

*   *Feature*: Add authentication support of provider
*   *Feature*: Add API documentation (doxygen)
*   *Feature*: Add `mountpoint_factory`
*   *Feature*: Add option to build adapter and provider libraries separately
*   *Fix*: Fix crash of example when using option `-h`
*   *Fix*: Fix error in `static_filesystem` providen when adding multiple files to same directory

## 0.1.0 _(Sat Apr 27 2019)_

*   initial version
