//
// Created by hal9000 on 9/1/23.
//

#ifndef UNTITLED_THREADINGOPERATIONS_H
#define UNTITLED_THREADINGOPERATIONS_H


#include <vector>
#include <stdexcept>
#include <memory>
#include <thread>
#include <type_traits>
#include <valarray>
#include <random>
#include "../LinearAlgebra/ParallelizationMethods.h"
using namespace LinearAlgebra;
using namespace std;


template<typename T>
class ThreadingOperations {

public:
    
    ThreadingOperations(ParallelizationMethod parallelizationMethod) : _parallelizationMethod(parallelizationMethod) {}
    
    template<typename ThreadJob>
    static void
    executeParallelJob(size_t size, ThreadJob task, unsigned availableThreads, unsigned cacheLineSize = 64) {
        unsigned doublesPerCacheLine = cacheLineSize / sizeof(double);
        unsigned int numThreads = std::min(availableThreads, static_cast<unsigned>(size));

        unsigned blockSize = (size + numThreads - 1) / numThreads;
        blockSize = (blockSize + doublesPerCacheLine - 1) / doublesPerCacheLine * doublesPerCacheLine;

        vector<thread> threads;
        for (unsigned int i = 0; i < numThreads; ++i) {
            unsigned start = i * blockSize;
            unsigned end = start + blockSize;
            if (start >= size) break;
            end = std::min(end, static_cast<unsigned>(size)); // Ensure 'end' doesn't exceed 'size'
            threads.push_back(thread(task, start, end));
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }

    /**
    * \brief Executes the provided task in parallel across multiple threads with a reduction step.
    * 
    * This method distributes the task across available CPU cores. Each thread operates on a distinct segment
    * of the data and produces a local result. After all threads have completed their work, a reduction step
    * combines these local results into a single global result.
    * 
    * \tparam T The data type of the result (e.g., double, float).
    * \tparam ThreadJob A callable object type (function, lambda, functor).
    *
    * \param size The size of the data being processed.
    * \param task The callable object that describes the work each thread should execute and return a local result.
     * \param availableThreads The number of threads available for processing.
    * \param cacheLineSize An optional parameter to adjust for system's cache line size (default is 64 bytes).
    * 
    * \return The combined result after the reduction step.
    */
    template<typename ThreadJob>
    static T executeParallelJobWithReduction(size_t size, ThreadJob task, unsigned availableThreads,
                                              unsigned cacheLineSize = 64) {
        unsigned doublesPerCacheLine = cacheLineSize / sizeof(double);
        unsigned int numThreads = std::min(availableThreads, static_cast<unsigned>(size));

        unsigned blockSize = (size + numThreads - 1) / numThreads;
        blockSize = (blockSize + doublesPerCacheLine - 1) / doublesPerCacheLine * doublesPerCacheLine;

        vector<T> localResults(numThreads);
        vector<thread> threads;

        for (unsigned int i = 0; i < numThreads; ++i) {
            unsigned start = i * blockSize;
            unsigned end = start + blockSize;
            if (start >= size) break;
            end = std::min(end, static_cast<unsigned>(size)); // Ensure 'end' doesn't exceed 'size'
            threads.push_back(thread([&](unsigned start, unsigned end, unsigned idx) {
                localResults[idx] = task(start, end);
            }, start, end, i));
        }

        for (auto &thread: threads) {
            thread.join();
        }

        T finalResult = 0;
        for (T val: localResults) {
            finalResult += val;
        }
        return finalResult;
    }

    /**
    * \brief Executes the provided task in parallel across multiple threads with an incomplete reduction step.
    * 
    * This method distributes the task across available CPU cores. Each thread operates on a distinct segment
    * of the data and produces a local result. After all threads have completed their work, a reduction step
    * combines these local results into a single global result.
    * 
    * \tparam T The data type of the result (e.g., double, float).
    * \tparam ThreadJob A callable object type (function, lambda, functor).
    *
    * \param size The size of the data being processed.
    * \param task The callable object that describes the work each thread should execute and return a local result.
     * \param availableThreads The number of threads available for processing.
    * \param cacheLineSize An optional parameter to adjust for system's cache line size (default is 64 bytes).
    * 
    * \return The result vector after the reduction step.
    */
    template<typename ThreadJob>
    static vector<T>
    executeParallelJobWithIncompleteReduction(size_t size, ThreadJob task, unsigned availableThreads,
                                               unsigned cacheLineSize = 64) {
        unsigned doublesPerCacheLine = cacheLineSize / sizeof(double);
        unsigned int numThreads = std::min(availableThreads, static_cast<unsigned>(size));

        unsigned blockSize = (size + numThreads - 1) / numThreads;
        blockSize = (blockSize + doublesPerCacheLine - 1) / doublesPerCacheLine * doublesPerCacheLine;

        vector<T> localResults(numThreads);
        vector<thread> threads;

        for (unsigned int i = 0; i < numThreads; ++i) {
            unsigned start = i * blockSize;
            unsigned end = start + blockSize;
            if (start >= size) break;
            end = std::min(end, static_cast<unsigned>(size)); // Ensure 'end' doesn't exceed 'size'
            threads.push_back(thread([&](unsigned start, unsigned end, unsigned idx) {
                localResults[idx] = task(start, end);
            }, start, end, i));
        }

        for (auto &thread: threads) {
            thread.join();
        }
        return localResults;
    }

    template<typename ThreadJob>
    void executeParallelJob(ThreadJob task) {
        if (_parallelizationMethod == SingleThread) {
            executeParallelJob(_values->size(), task, 1);
        } else if (_parallelizationMethod == MultiThread) {
            executeParallelJob(_values->size(), task, std::thread::hardware_concurrency());
        }
    }

    template<typename ThreadJob>
    double executeParallelJobWithReductionForDoubles(ThreadJob task) {
        if (_parallelizationMethod == SingleThread) {
            return executeParallelJobWithReduction(_values->size(), task, 1);
        } else if (_parallelizationMethod == MultiThread) {
            return executeParallelJobWithReduction(_values->size(), task, std::thread::hardware_concurrency());
        }
    }


    /**
    * @brief Performs a deep copy from the source to the current object.
    * 
    * This method uses the dereference_trait to handle various types of sources 
    * such as raw pointers, unique pointers, shared pointers, and direct objects.
    * 
    * @param source The source object to be copied from.
    */
    template<typename InputType>
    void _deepCopy(const InputType &source) {

        if (size() != dereference_trait<InputType>::size(source)) {
            throw std::invalid_argument("Source vector must be the same size as the destination vector.");
        }

        const T *sourceData = dereference_trait<InputType>::dereference(source);

        auto deepCopyThreadJob = [&](unsigned start, unsigned end) {
            for (unsigned i = start; i < end && i < _values->size(); ++i) {
                (*_values)[i] = sourceData[i];
            }
        };
        _executeParallelJob(deepCopyThreadJob);
    }

    /**
    * @brief Checks if the elements of the current object are equal to those of the provided source.
    * 
    * This method uses parallelization to perform the comparison and then reduces the results to determine 
    * if all elements are equal.
    * 
    * @param source The source object to be compared with.
    * @return true if all elements are equal, false otherwise.
    */
    bool _areElementsEqual(const T *&source, size_t size) {

        if (_values->size() != source->size()) {
            throw std::invalid_argument("Source vector must be the same size as the destination vector.");
        }

        auto compareElementsJob = [&](unsigned start, unsigned end) -> bool {
            for (unsigned i = start; i < end && i < _values->size(); ++i) {
                if ((*_values)[i] != source[i]) {
                    return false;
                }
            }
            return true;
        };

        // Check elements in parallel and reduce the results
        if (_parallelizationMethod == SingleThread) {
            return _executeParallelJobWithReduction(_values->size(), compareElementsJob, 1);
        }

        if (_parallelizationMethod == MultiThread) {
            return _executeParallelJobWithReduction(_values->size(), compareElementsJob,
                                                    std::thread::hardware_concurrency());
        }
    }
    
    private:
    ParallelizationMethod _parallelizationMethod;
}

#endif //UNTITLED_THREADINGOPERATIONS_H
