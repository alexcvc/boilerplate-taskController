#pragma once

#include <chrono>
#include <vector>

#include <cppsl/time/stopTimer.hpp>

namespace cppsl::time {

/**
 * This is a C++ template class called "RoundWatch" that extends the "StopTimer" class with a default template parameter
 * of "std::chrono::milliseconds".
 * The "RoundWatch" class has a few type aliases for convenience:
 * "clock" and "time_point" are aliases for the clock and time_point types from the "StopTimer" class.
 * The "State" alias is for the "State" enum from the "StopTimer" class.
 *
 * The class also has a nested struct called "LapDurations" which contains two durations: "total_time" and "split_time".
 *
 * The class provides a method called "StoreLap" which stores the lap durations if the clock state is running.
 * The lap durations are calculated by subtracting the previous lap's total time from the current total time.
 * The lap durations are then stored in a vector.
 *
 * The class also provides a method called "Reset" which clears the vector of lap durations.
 * Lastly, the class has a method called "Laps" which returns a const reference to the vector of lap durations.
 */
template <typename duration = std::chrono::milliseconds>
class RoundWatch : public StopTimer<duration> {
  using ClockType = typename StopTimer<duration>::Clock;
  using TimePointType = typename StopTimer<duration>::TimePoint;

 public:
  RoundWatch() = default;

  /**
   * @brief This struct represents the lap durations of a stopwatch.
   *
   * The LapDurations struct contains two duration values: total_time and split_time.
   * The total_time represents the total elapsed time since the stopwatch started,
   * and the split_time represents the time elapsed since the last lap.
   */
  struct LapDurations {
    duration total_time;
    duration split_time;
  };

  /**
    * @brief Stores the lap durations if the timer is running.
    *
    * If the timer (StopTimer) is running, this function calculates the current lap duration
    * by subtracting the previous lap's total time from the current total time. It then stores
    * the calculated lap duration in a vector (lap_durations_).
    */
  void StoreLap() {
    if (StopTimer<duration>::IsRunning()) {
      lap_durations_.push_back(CalculateCurrentLapDuration());
    }
  }

  /**
   * @brief Resets the RoundWatch object by clearing the vector of lap durations.
   *
   * This function resets the RoundWatch object by clearing the vector of lap durations. After calling this function,
   * the RoundWatch object will have no lap durations stored.
   *
   * @see Laps()
   */
  void Reset() noexcept {
    lap_durations_.clear();
  }

  /**
   * Returns a const reference to the vector of lap durations.
   *
   * @return The const reference to the vector of lap durations.
   */
  const std::vector<LapDurations>& Laps() const {
    return lap_durations_;
  }

 private:
  std::vector<LapDurations> lap_durations_;

  /**
   * @brief Calculates the current lap duration.
   *
   * This function calculates the current lap duration based on the total time elapsed since the stopwatch started.
   * It returns a struct called LapDurations, which contains two durations: total_time and split_time.
   * The total_time is the current total time elapsed, and the split_time is the time elapsed since the last lap.
   * If there are no previous lap durations, the split_time is equal to the total_time.
   *
   * @return The current lap duration.
   */
  LapDurations CalculateCurrentLapDuration() {
    LapDurations current;
    current.total_time = StopTimer<duration>::ElapsedTime();

    if (lap_durations_.empty()) {
      current.split_time = current.total_time;
    } else {
      current.split_time = current.total_time - lap_durations_.back().total_time;
    }

    return current;
  }
};

}  // namespace cppsl::time
