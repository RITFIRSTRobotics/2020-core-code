add_library(Collections arraylist.c hashtable.c linkedlist.c list.c queue.c stack.c)
target_link_libraries(Collections pthread)
