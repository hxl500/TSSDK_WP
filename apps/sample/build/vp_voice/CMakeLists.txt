set (SAMPLE_COMMON_DIR      ${APP_SRC_DIR}/common)
set (VP_VOICE_DIR           ${APP_SRC_DIR}/vp_voice)
set (VP_COMM_DIR            ${APP_SRC_DIR}/vp_comm)

include_directories(
    ${VP_VOICE_DIR}
    ${VP_COMM_DIR}/inc/vp_tools
    ${VP_COMM_DIR}/inc/vp_printf
    ${VP_COMM_DIR}/inc/vp_events
    ${VP_COMM_DIR}/inc/vp_context
    ${VP_COMM_DIR}/inc/vp_observers
)

link_directories(
    ${VP_COMM_DIR}/lib/vp_tools
    ${VP_COMM_DIR}/lib/vp_printf
    ${VP_COMM_DIR}/lib/vp_events
    ${VP_COMM_DIR}/lib/vp_context
    ${VP_COMM_DIR}/lib/vp_observers
)

set(VP_VOICE_TEST vp_voice_test)

add_executable(
    ${VP_VOICE_TEST}
    ${SAMPLE_COMMON_DIR}/sample_comm_sys.c
    ${VP_VOICE_DIR}/vp_voice_ts.c
    ${VP_VOICE_DIR}/test.c
)

set (VQE_LIB
    libalgcommon.a
)

set (VP_LIB
    libvp_tools.a
    libvp_observers.a
    libvp_printf.a
    libvp_context.a
)

TARGET_LINK_LIBRARIES(${VP_VOICE_TEST} -Wl,--start-group dl pthread stdc++
    ${VQE_LIB} ${VP_LIB} libalgwebrtc3a.a libalgmmsens.a libalgomlsa.a libalgnoisegate.a libalgsolvqestereo.a libalgadaptaec.a libNE10_intrinsic.a libalgcommon.a ${MPI_BASE_LIB} -Wl,--end-group)
