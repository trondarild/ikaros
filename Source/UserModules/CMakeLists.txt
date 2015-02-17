set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(LinAlgTest)
add_subdirectory(SparseMatrix)
add_subdirectory(ReservoirNetwork)
add_subdirectory(ScalarCollector)
add_subdirectory(ArrayCollector)
add_subdirectory(MatrixCollector)
add_subdirectory(MinimalModule)
add_subdirectory(MyModule)

set(SOURCE ${SOURCE} PARENT_SCOPE)
set(MODULES_INCLUDS ${MODULES_INCLUDS} PARENT_SCOPE)
set(MODULES_LIBS ${MODULES_LIBS} PARENT_SCOPE)