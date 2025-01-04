#ifndef MEDIANFILTER_H
#define MEDIANFILTER_H

#include <vector>
#include <deque>

class MedianFilter {
private:
    std::vector<unsigned long> sortedArray;  // Sorted array to store values for median calculation
    std::deque<unsigned long> timeQueue;     // Queue to track insertion order (FIFO)
    int maxSize;                             // Maximum size of the array

public:
    // Constructor: initializes the sorted array and queue with a specified size
    MedianFilter(int size, unsigned long initialValue);

    // Insert a new value and maintain the sorted array
    void insertValue(unsigned long value);

    // Get the median value from the sorted array
    unsigned long getMedian();
};

#endif // MEDIANFILTER_H
