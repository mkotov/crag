#Specify the version being used aswell as the language
cmake_minimum_required(VERSION 2.8.12)
cmake_policy(VERSION 2.8.12)

include("../cmake/common.cmake")

crag_library(SbgpFG
  SubgroupFG
  WhiteheadGraph
)

target_link_libraries(SbgpFG
  PUBLIC Elt
  PUBLIC Graph
)

if (UNIX)
  target_compile_options(SbgpFG PRIVATE "-fpermissive")
endif()

#target_link_libraries(CryptoTripleDecomposition
#  PUBLIC BraidGroup
#  PRIVATE ranlib
#)
