#include "threadpool.h"
#include <pthread.h>
#include <stdbool.h>
#include <iostream>
#include <cassert>
#include <queue>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>

ThreadPool_t *ThreadPool_create(int num)
{
    assert(num > 0);
    ThreadPool_t *tp = new ThreadPool_t;

    // Check if malloc return NULL, terminate if NULL
    if (tp == NULL)
    {
        printf("Error! Memory not allocated.");
        delete tp;
        return NULL;
    }

    // Thread init
    pthread_cond_init(&tp->work_arrived, NULL);
    pthread_mutex_init(&tp->mutex, NULL);
    tp->mStoppping = false;

    ThreadPool_work_queue_t *jobQueue = new ThreadPool_work_queue_t;
    // Check if malloc return NULL, terminate if NULL
    if (jobQueue == NULL)
    {
        printf("Error! Memory not allocated.");
        ThreadPool_destroy(tp);
        return NULL;
    }

    tp->jobQueue = jobQueue;

    // Creating 'num' threads
    for (int i = 0; i < num; i++)
    {
        pthread_t *thread = new pthread_t;
        pthread_create(thread, NULL, (void *(*)(void *))Thread_run, tp);
        tp->mThreads.push_back(thread);
    }

    // Return threadpool
    return tp;
}

void ThreadPool_destroy(ThreadPool_t *tp)
{
    // Check for evrything to be done
    // Check if queue is empty or not
    for (;;)
    {
        pthread_mutex_lock(&tp->mutex);
        if (tp->jobQueue->queue.empty())
        {
            break;
        }
        // Job Queue still has tasks, let the threads work.
        pthread_mutex_unlock(&tp->mutex);
    }

    // Wait for all threads to finish
    // Wake up workers to make them see that mStoppping has changed
    tp->mStoppping = true;
    pthread_cond_broadcast(&tp->work_arrived);
    pthread_mutex_unlock(&tp->mutex);

    // Wait for threads to be done
    // Free and delete threads in vector
    while (!tp->mThreads.empty())
    {
        pthread_join(*tp->mThreads.back(), NULL);
        tp->mThreads.pop_back();
    }

    // Destory conditional variables and mutexes
    // Lock mutex incase
    pthread_mutex_lock(&tp->mutex);
    pthread_cond_destroy(&tp->work_arrived);
    pthread_mutex_destroy(&tp->mutex);

    // Free jobQueue and threadpool
    delete tp->jobQueue;
    delete tp;
}

bool ThreadPool_add_work(ThreadPool_t *tp, thread_func_t func, void *arg)
{
    ThreadPool_work_t *work = new ThreadPool_work_t;

    // Assign work
    work->func = func;
    work->arg = arg;

    // Add work to queue
    pthread_mutex_lock(&tp->mutex);
    tp->jobQueue->queue.push_back(work);
    // Signal new work
    pthread_cond_signal(&tp->work_arrived);
    pthread_mutex_unlock(&tp->mutex);
    return true;
}

ThreadPool_work_t *ThreadPool_get_work(ThreadPool_t *tp)
{
    // Return a task from queue
    return tp->jobQueue->queue.front();
    // Remember to pop it.
}

void *Thread_run(ThreadPool_t *tp)
{
    for (;;)
    {
        // Get the next work item
        ThreadPool_work_t *nextTask;
        // Lock mutex
        pthread_mutex_lock(&tp->mutex);

        // If queue is empty and threadpool not called to stop, we wait.
        // We own the lock while waiting for more work.
        // Because there are no other jobs, no point giving the lock to other threads
        while (tp->jobQueue->queue.empty() && (!tp->mStoppping))
        {
            pthread_cond_wait(&tp->work_arrived, &tp->mutex);
        }

        // Stop the thread, no more work to be done
        if (tp->mStoppping)
        {
            break;
        }

        // Make sure Thread gets the work
        // Get work
        nextTask = ThreadPool_get_work(tp);

        // Pop the work item
        tp->jobQueue->queue.erase(tp->jobQueue->queue.begin());        

        // Unlock mutex
        pthread_mutex_unlock(&tp->mutex);

        // Do the work item
        nextTask->func(nextTask->arg);
    }
    // Unlock mutex, so others can terminate
    pthread_mutex_unlock(&tp->mutex);
    return NULL;
}