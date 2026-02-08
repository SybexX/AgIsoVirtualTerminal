if(NOT TARGET isobus::isobus)
  include(FetchContent)
  FetchContent_Declare(
    CAN_Stack
    GIT_REPOSITORY https://github.com/SybexX/AgIsoStack-plus-plus.git
    GIT_TAG 506de66352c14c4c074a00115a67db0f4752e390)
  FetchContent_MakeAvailable(CAN_Stack)
endif()
