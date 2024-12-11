#ifndef TIMESYNC_H
#define TIMESYNC_H

// Raghav: I do not have the dependencies for this right now but it should work
#include <vector>
#include <array>
#include <utility>
#include <algorithm>

class TimeSync {
public:
    // Constructor: max_history_size controls how many readings are stored
    TimeSync(size_t max_history_size = 100);

    // Add a new sensor reading (timestamp in ms and values as array)
    void addReading(double timestamp, const std::array<double, 3>& values);

    // Get interpolated values at the desired time
    std::array<double, 3> getValuesAt(double desired_time) const;

private:
    size_t max_history_size;
    std::vector<std::pair<double, std::array<double, 3>>> readings; // {timestamp, {values}}

    // Helper to find the two closest timestamps to the desired time
    std::pair<std::pair<double, std::array<double, 3>>, std::pair<double, std::array<double, 3>>> getClosestReadings(double desired_time) const;
};

#endif // TIMESYNC_H
