set(Catch2_DIR ${CMAKE_SOURCE_DIR}/external/Catch2)


MESSAGE(STATUS "Fetch Content [Catch2][${Catch2_DIR}]")

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY git@hming.wang:github/Catch2.git
  GIT_TAG        v3.8.0 # or a later release
  SOURCE_DIR     ${Catch2_DIR}
)

FetchContent_MakeAvailable(Catch2)