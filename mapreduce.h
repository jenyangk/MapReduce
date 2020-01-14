#ifndef MAPREDUCE_H
#define MAPREDUCE_H

// function pointer types used by library functions
typedef void (*Mapper)(char *file_name);
typedef void (*Reducer)(char *key, int partition_number);


/**
 * @brief Start the mapping function.
 *
 * @param num_files - Number of files (K)
 * @param filenames - Array of filenames containing K data splits 
 * @param map - Function pointer to Map
 * @param num_mappers - Number of mappers
 * @param concate - Function pointer to Reduce
 * @param num_reducers - Number of reducers
 * 
 * Library functions you must define.
 * The main thread that runs MR_RUN is the master thread.
 * It creates M mappers and R reducers (i.e. in Figure 1)
 */

void MR_Run(int num_files, char *filenames[],
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers);

/**
 * Takes a key and a value associated with it,
 * and writes this pair to a specific partition which is determined
 * by passing the key to the MR_Partition lib function.
 * This function can be any good hash function (e.g. CRC32, MurmurHash)
 * or following alg known as DJB2.
 */

void MR_Emit(char *key, char *value);

// Hash Function
unsigned long MR_Partition(char *key, int num_partitions);

/**
 * The MR ProcessPartition library function takes the index of 
 * the partition assigned to the thread that runs
 * it. It invokes the user-defined Reduce function in a loop, 
 * each time passing it the next unprocessed key. This
 * continues until all keys in the partition are processed.
 */
void MR_ProcessPartition(void *partition_number);

/**
 * The MR GetNext library function takes a key and a partition 
 * number, and returns a value associated with the key
 * that exists in that partition. In particular, the ith call to 
 * this function should return the ith value associated with the
 * key in the sorted partition or NULL if i is greater than the 
 * number of values associated with the key.
 */
char *MR_GetNext(char *key, int partition_number);
#endif