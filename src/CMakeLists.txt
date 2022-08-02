set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

add_executable(server main.cpp Server.cpp Connection.cpp Logger.cpp Thread.cpp Transport.cpp)
target_link_libraries(server PRIVATE WNET Threads::Threads)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/WNET/include)

install(TARGETS server DESTINATION bin)
