#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <algorithm>

#include "threadpool.h"

// function pointer types used by library functions
typedef void (*Mapper)(char *file_name);
typedef void (*Reducer)(char *key, int partition_number);

// Number of partitions created by mappers
int partitions_num;

// Intermediate Data Structure
std::vector<std::map<std::string, std::vector<std::string>>> partitions;

Reducer reduce;
ThreadPool_t *poolMap;

// Functor that compares file sizes (sort largest to smallest)
bool file_comparator(char *A, char *B)
{
    struct stat stat_A, stat_B;
    stat(A, &stat_A);
    stat(B, &stat_B);
    return stat_A.st_size > stat_B.st_size;
}

// Takes the index of the partition assigned to the thread that runs
// Invokes the user-defined Reduce function in a loop, each time passing it the next unprocessed key. This
// continues until all keys in the partition are processed.
void MR_ProcessPartition(void *partition_number)
{
    int partNum = *((int *)partition_number);

    // Go through keys
    std::map<std::string, std::vector<std::string>>::iterator it = partitions[partNum].begin();

    while (it != partitions[partNum].end())
    {
        // Invoke Reduce
        reduce((char *)it->first.c_str(), partNum);
        it++;
    }
    delete (int *) partition_number;
}

// Returns value that is associated with the key at Partition 'partition_number'
char *MR_GetNext(char *key, int partition_number)
{
    std::string keyString = key;
    std::string valueString;
    // If the partition number doesn't exists, return NULL
    if (!partitions[partition_number].at(keyString).empty())
    {
        valueString = partitions[partition_number].at(keyString).front();
        partitions[partition_number].at(keyString).erase(partitions[partition_number].at(keyString).begin());
        return (char *)valueString.c_str();
    }
    // Key at Partition number not found
    return NULL;
}

void MR_Run(int num_files, char *filenames[], Mapper map, int num_mappers, Reducer concate, int num_reducers)
{
    // Create 'num_mappers' mappers
    poolMap = ThreadPool_create(num_mappers);
    partitions_num = num_reducers;
    partitions.resize(num_reducers);
    reduce = concate;
    
    // Check if Threadpool creation failed.
    if (poolMap == NULL)
    {
        std::cout << "Threadpool creation failed.";
        exit(EXIT_FAILURE);
    }

    // Sort filenames (largest file first)
    std::sort(filenames, filenames + num_files - 1, file_comparator);

    // Produce work items for mappers
    int i = 0;
    while (i < num_files)
    {
        // Add work
        ThreadPool_add_work(poolMap, (thread_func_t)map, filenames[i]);
        // Signal that work has arrived
        pthread_cond_signal(&poolMap->work_arrived);
        // Threads should be processing work at this point
        i++;
    }
    
    // Destroy the threads
    ThreadPool_destroy(poolMap);

    // Produce work items for reducer
    // pthread_t reducers[num_reducers];
    for (int i = 0; i < num_reducers; i++)
    {
        int *num = new int;
        *num = i;
        pthread_t thread;
        pthread_create(&thread, NULL, (void *(*)(void *))MR_ProcessPartition, (void *)num);
        pthread_detach(thread);
    }
    pthread_exit(NULL);

}

// Gives a partition number
unsigned long MR_Partition(char *key, int num_partitions)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *key++) != '\0')
        hash = hash * 33 + c;
    return hash % num_partitions;
}

void MR_Emit(char *key, char *value)
{
    // Key and Value in String form
    std::string keyString = key;
    std::string valueString = value;

    // Put items in a Intermediate Data Structure
    unsigned long partition_key = MR_Partition(key, partitions_num);
    // Insert key & value pair to some partition
    pthread_mutex_lock(&poolMap->mutex);
    int check = partitions[partition_key].count(keyString);
    // if key exists, at the value to the vector
    if (check > 0)
    {
        
        partitions[partition_key][keyString].push_back(valueString);
    }
    else
    {
        // Insert new key
        partitions[partition_key].insert(std::pair<std::string, std::vector<std::string>>(keyString, {valueString}));
        
    }
    pthread_mutex_unlock(&poolMap->mutex);
}
