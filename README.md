# Assignment 2

Information:

Name: Koh Jen Yang

CCID: jenyang



## Intermediate Key-Value Pair Storage

Since the user will be providing our program with the number of reducers (*r*), we create a `Vector` of size *r*. Thus, each element of the `Vector` would correspond to a intermediate key-value pair because the hashed value in `MR_Partition` determines which Partition the key will belong to. 



## Time Complexity Analysis

- MR_Emit
  - Suppose there are **n** keys in total.
  - For each word, the `MR_Emit` will call `MR_Partition`, `count ` and `insert()` if the key doesn't exists or `push_back()` if the key exists.
  - MR_Partition takes O(n) time, `count` takes O(log n), `insert()` takes O(log n) and `push_back()` takes O(1) time.
  - Thus, the run time of `MR_Emit` is O(n + log n + log n) or O(n + log n)
- `MR_GetNext`
  - Suppose there are `n` keys in total
  - For each word, the `MR_GetNext` will call:
    - `at()` takes O(log n) time, `empty()` takes O(1) time.
    - `at()` takes O(log n) time, `front()` takes O(1) time.
    - `at()` takes O(log n) time, `erase()` takes O(n) time, `at()` takes O(log n) time, `begin()` takes O(1) time.
  - Thus, the run time is O(log n + log n + n log n)

## Thread pool



## Task Queue

For the task queue, a `Vector` data structure was used to add, get and remove tasks. Since the files have already been sorted based on their file size (`stat` syscall), we can then just add the `push_back()` the files in descending order sizes. The Mapper threads will then take and pop the first task in the `Vector` task queue.

## Synchronization Primitives

`pthread_join`: Used to wait for all Mapper threads to terminate.

`pthread_cond_wait`: Used to put a Mapper thread to sleep as there might be no tasks in the task queue.

`pthread_cond_signal`: Used to signal Mapper threads that are sleeping to wakeup. 

`pthread_cond_broadcast`: Used to signal Mapper threads that are waiting on the `cond` condition variable to wakeup. 

`pthread_mutex_lock`: Used to make sure only one Mapper thread access a shared data at that moment in time.

`pthread_mutex_unlock`: Used to allow other Mapper threads to access the shared data.

## Testing

As for testing the correctness of the code, `mapreduce.cpp` , `threadpool.cpp` and its respective header files were tested with the tests cases provided by the Lab TAs. The `easy`, `medium`, and `hard` by *Jihoon Og* was used to test my program. The correctness was determined by the final result of the test script ("Congratulation, your mapreduce library is concurrency stable" shows that the program provided a valid result).

-----

## Sources

C++ Map:

http://www.cplusplus.com/reference/map/map/

*By The C++ Resources Network, 2019*



C++ Vector:

http://www.cplusplus.com/reference/vector/vector/

*By The C++ Resources Network, 2019*



C++ String:

http://www.cplusplus.com/reference/string/string/

*By The C++ Resources Network, 2019*



Portions of this project are modifications based on work created and shared by the Lab Instructors:

Pthreads, Conditional Variables, Mutexes

[*Lab 4*](https://eclass.srv.ualberta.ca/mod/resource/view.php?id=3842777) and [*Lab 5*](https://eclass.srv.ualberta.ca/mod/resource/view.php?id=3848414)



Portions of this project are modifications based on work created and shared by the Wikimedia Project and used according to terms described in the Creative Commons Attribution - ShareAlike License:

Threading:

https://en.wikibooks.org/wiki/C%2B%2B_Programming/Threading

*By Wikibooks, last edited last edited on March 22, 2019.*



Synchronization:

https://en.wikibooks.org/wiki/Embedded_Systems/Threading_and_Synchronization

*By Wikibooks, last edited last edited on December 10, 2018.*



