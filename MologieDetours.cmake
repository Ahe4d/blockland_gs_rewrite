set(MologieDetours ${CMAKE_CURRENT_LIST_DIR}/MologieDetours)

include_directories(
  ${MologieDetours}/
)

add_library(MologieDetours STATIC ${MologieDetours}/hde.cpp)
