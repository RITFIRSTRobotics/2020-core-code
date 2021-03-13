include_directories(../collections)
add_library(Network lowlevel.c messagehandler.c netutils.c packethandlers.c)
target_link_libraries(Network pthread m)
