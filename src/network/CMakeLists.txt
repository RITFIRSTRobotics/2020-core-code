include_directories(../collections)
add_library(Network OBJECT lowlevel.c messagehandler.c netutils.c packethandlers.c)
target_link_libraries(Network pthread m)
