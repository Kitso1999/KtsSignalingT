add_library(KtsSignalingT INTERFACE)

target_include_directories(KtsSignalingT INTERFACE
    $<BUILD_INTERFACE:${KtsSignalingT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_sources(KtsSignalingT PUBLIC FILE_SET HEADERS FILES signaling_t.hpp )

add_library(Kts::SignalingT ALIAS KtsSignalingT)
