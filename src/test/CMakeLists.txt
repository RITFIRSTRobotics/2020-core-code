add_executable(test-hashtable test-hashtable.c)
target_link_libraries(test-hashtable Collections)

add_executable(test-list test-list.c)
target_link_libraries(test-list Collections pthread m)

add_executable(test-llnet test-llnet.c)
target_link_libraries(test-llnet Collections Network pthread)
configure_file(llnet.valgrind.supp llnet.valgrind.supp COPYONLY)

add_executable(test-packethandlers test-packethandlers.c)
target_link_libraries(test-packethandlers Collections Network)
