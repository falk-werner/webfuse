# timer

add_library(wf_timer STATIC
    lib/wf/timer/src/wf/timer/api.c
    lib/wf/timer/src/wf/timer/impl/manager.c
    lib/wf/timer/src/wf/timer/impl/timepoint.c
    lib/wf/timer/src/wf/timer/impl/timer.c
)

target_include_directories(wf_timer PRIVATE 
	lib/wf/timer/src
)

target_include_directories(wf_timer PUBLIC 
	lib/wf/timer/include
)

set_target_properties(wf_timer PROPERTIES C_VISIBILITY_PRESET hidden)
