#Specify the version being used aswell as the language
cmake_minimum_required(VERSION 2.8.12)
cmake_policy(VERSION 2.8.12)

include("../cmake/common.cmake")

crag_library(Equation
  Equation
)

target_link_libraries(Equation
  PUBLIC Elt
  PUBLIC Graph
  PRIVATE ranlib
)

#target_link_libraries(CryptoTripleDecomposition
#  PUBLIC BraidGroup
#  PRIVATE ranlib
#)
