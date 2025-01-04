#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include "MedianFilter.h"

MedianFilter::MedianFilter(int size, unsigned long initialValue) 
    : maxSize(size), sortedArray(), timeQueue() {
    // Initialize sorted array and queue with the initial value
    sortedArray.reserve(size);
    timeQueue.push_back(initialValue);
}

// Insert a new value and remove the oldest one in the same operation
void MedianFilter::insertValue(unsigned long value) {
    // Remove the oldest value from both the queue and the sorted array
    unsigned long oldestValue = timeQueue.front();
    timeQueue.pop_front();

    // Find and erase the oldest value from the sorted array
    auto pos = std::lower_bound(sortedArray.begin(), sortedArray.end(), oldestValue);
    if (pos != sortedArray.end() && *pos == oldestValue) {
        sortedArray.erase(pos);
    }

    // Add the new value to both the queue and the sorted array
    timeQueue.push_back(value);

    // Insert the new value in the correct position in the sorted array
    auto insertPos = std::lower_bound(sortedArray.begin(), sortedArray.end(), value);
    sortedArray.insert(insertPos, value);
}

// Get the median value from the sorted array
unsigned long MedianFilter::getMedian() {
    int n = sortedArray.size();
    if (n == 0) {
        return 0;  // Handle empty case (you might want to handle it differently)
    }
    // If odd number of elements, return the middle one
    if (n % 2 == 1) {
        return sortedArray[n / 2];
    } 
    // If even number of elements, return the average of the two middle ones
    else {
        return (sortedArray[n / 2 - 1] + sortedArray[n / 2]) / 2;
    }
}
