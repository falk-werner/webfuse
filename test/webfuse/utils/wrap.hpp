#ifndef WF_WRAP_HPP
#define WF_WRAP_HPP

#define WF_WRAP_FUNC0( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (); \
    RETURN_TYPE __wrap_ ## FUNC_NAME () \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(); \
        } \
    }

#define WF_WRAP_FUNC1( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1); \
        } \
    }

#define WF_WRAP_FUNC2( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE, ARG2_TYPE); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1, ARG2_TYPE arg2) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1, arg2); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1, arg2); \
        } \
    }

#define WF_WRAP_FUNC3( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE, ARG2_TYPE, ARG3_TYPE); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1, arg2, arg3); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1, arg2, arg3); \
        } \
    }

#define WF_WRAP_FUNC4( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1, arg2, arg3, arg4); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1, arg2, arg3, arg4); \
        } \
    }

#define WF_WRAP_FUNC5( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1, arg2, arg3, arg4, arg5); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1, arg2, arg3, arg4, arg5); \
        } \
    }

#define WF_WRAP_FUNC6( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, ARG6_TYPE); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, ARG6_TYPE arg6) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1, arg2, arg3, arg4, arg5, arg6); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1, arg2, arg3, arg4, arg5, arg6); \
        } \
    }


#define WF_WRAP_VFUNC3( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, va_list); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, va_list args) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1, arg2, arg3, args); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1, arg2, arg3); \
        } \
    }

#define WF_WRAP_VFUNC5( GLOBAL_VAR, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE ) \
    extern RETURN_TYPE __real_ ## FUNC_NAME (ARG1_TYPE, ARG2_TYPE, ARG3_TYPE, ARG4_TYPE, ARG5_TYPE, va_list); \
    RETURN_TYPE __wrap_ ## FUNC_NAME (ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3, ARG4_TYPE arg4, ARG5_TYPE arg5, va_list args) \
    { \
        if (nullptr == GLOBAL_VAR ) \
        { \
            return __real_ ## FUNC_NAME (arg1, arg2, arg3, arg4, arg5, args); \
        } \
        else \
        { \
            return GLOBAL_VAR -> FUNC_NAME(arg1, arg2, arg3, arg4, arg5); \
        } \
    }


#endif
