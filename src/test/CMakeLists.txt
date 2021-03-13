# TODO this shouldnt be a library
add_executable(test-hashtable test-hashtable.c)
target_link_libraries(test-hashtable Collections)
add_executable(test-list test-list.c)
target_link_libraries(test-list Collections pthread m)
add_executable(test-llnet test-llnet.c)
target_link_libraries(test-llnet Collections Network pthread)
add_executable(test-packethandlers test-packethandlers.c)
target_link_libraries(test-packethandlers Collections Network)
