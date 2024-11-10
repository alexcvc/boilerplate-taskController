/************************************************************************/ /**
 * @file
 * @brief  chrono functions
 *
 *   How to use:
 *   ---
 *
 *   using std::chrono::system_clock;
 *   auto now = system_clock::now();
 *
 *   std::clog << "==== " << TimePointToString(now) << std::endl;
 *
 *   using DD = std::chrono::duration<std::size_t, std::ratio<2, 3>>;
 *   using TP = std::chrono::time_point<std::chrono::system_clock, DD>;
 *
 *   for (int i = 0; i <= 3; ++i)
 *       std::clog << "==== " << TimePointToString(TP(DD(i))) << std::endl;
 *
 *   for (std::string s: {
 *                "2017-May-01 00:10:15",
 *                "2017-May-01 00:10:15.25",
 *                "2017-Mar-01",
 *                "1969-Dec-31 19:00:00.666666666666667",
 *                "2018",      // underspecified - but succeeds as 2017-12-31
 *                "1752-May-5",// out of range
 *                "not a date",
 *        }) {
 *       try {
 *           std::clog << "---- "
 *                     << TimePointToString(TimePointFromString<system_clock::time_point>(s))
 *                     << std::endl;
 *       } catch (const std::exception &e) {
 *           std::cerr << e.what() << std::endl;
 *       }
 *   }
 *
 *  ==== 2023-Jun-25 19:27:52.117357015609741
 *  ==== 1970-Jan-01 01:00:00.000000000000000
 *  ==== 1970-Jan-01 01:00:00.666666666666667
 *  ==== 1970-Jan-01 01:00:01.333333333333333
 *  ==== 1970-Jan-01 01:00:02.000000000000000
 *  ---- 2017-May-01 01:10:15.000000000000000
 *  ---- 2017-May-01 01:10:15.249999761581421
 *  ---- 2017-Mar-01 00:00:00.000000000000000
 *  ---- 1969-Dec-31 19:00:00.666666665998491
 *  ---- 2017-Dec-31 00:00:00.000000000000000
 *  ---- get_time
 *  ---- get_time
 *
****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::time {

/**
 * @brief Convert a time point to seconds and fractional seconds.
 *
 * This function converts a given time point to seconds and fractional seconds.
 * The time point can be of any duration type. The returned values are represented as a tuple
 * containing two doubles - the seconds and the fractional part of seconds.
 *
 * @tparam Double The type of the doubles to represent seconds and fractional seconds.
 * @tparam TimePoint The type of the time point to convert.
 * @param timePoint The time point to convert to seconds and fractional seconds.
 * @return std::tuple<Double, Double> A tuple containing the seconds and fractional seconds.
 */
template <typename Double, typename TimePoint>
inline std::tuple<Double, Double> TimePntToSecondsAndFractional(const TimePoint& timePoint) {
  Double seconds = Double(timePoint.time_since_epoch().count()) * TimePoint::period::num / TimePoint::period::den;
  Double fractionalSeconds = std::modf(seconds, &seconds);
  return {seconds, fractionalSeconds};
}

/**
 * @brief Formats the given time into a specified date and time format.
 *
 * This function takes a `std::ostringstream` object as the output stream,
 * a pointer to a `std::tm` object representing the time,
 * a `std::string` specifying the date format,
 * a `double` representing the fractional seconds,
 * and a `std::size_t` specifying the precision of the fractional seconds.
 *
 * The function formats the time in the given date format using `std::put_time`,
 * and appends the formatted date to the output stream.
 * It then appends the fractional seconds with the specified precision to the output stream.
 *
 * @param oss The output `std::ostringstream` object.
 * @param tm A pointer to a `std::tm` object representing the time.
 * @param dateFormat The date format string.
 * @param fractionalSeconds The fractional seconds.
 * @param precision The precision of the fractional seconds.
 *
 * @return None.
 */
inline void FormatTimeToStream(std::ostringstream& oss, const std::tm* tm, const std::string& dateFormat,
                               double fractionalSeconds, std::size_t precision) {
  oss << std::put_time(tm, dateFormat.c_str()) << std::setw(precision + 3) << std::setfill('0') << std::fixed
      << std::setprecision(precision) << tm->tm_sec + fractionalSeconds;
}

/**
 * Converts a given time point to a string representation with specified precision.
 *
 * @tparam Double The desired floating point type for representing seconds.
 * @tparam Precision The desired precision for fractional seconds.
 * @tparam TimePoint The type of the time point to convert.
 *
 * @param timePoint The time point to convert to a string.
 *
 * @return A string representation of the given time point.
 *
 * @throws std::runtime_error if an error occurs during conversion.
 */
template <typename Double = double, std::size_t Precision = std::numeric_limits<Double>::digits10, typename TimePoint>
  requires std::is_floating_point_v<Double> && (Precision <= std::numeric_limits<Double>::digits10)
inline std::string TimePointToString(const TimePoint& timePoint) {
  const std::string dateFormat = "%Y-%b-%d %H:%M:";
  const auto [wholeSeconds, fractionalSeconds] = TimePntToSecondsAndFractional<Double, TimePoint>(timePoint);
  std::time_t tt = static_cast<std::time_t>(wholeSeconds);

  std::ostringstream oss;
  auto tm = std::localtime(&tt);
  if (!tm)
    throw std::runtime_error(std::strerror(errno));

  FormatTimeToStream(oss, tm, dateFormat, fractionalSeconds, Precision);

  if (!oss)
    throw std::runtime_error("time-point-to-string");

  return oss.str();
}

/**
 * Converts a string representation of a date and time to a TimePoint object.
 *
 * @param str The string to convert.
 * @tparam TimePoint The type of TimePoint to create.
 * @return The TimePoint object representing the specified date and time.
 * @throws std::invalid_argument If the input string is in an invalid format.
 */
template <typename TimePoint>
TimePoint TimePointFromString(const std::string& str) {
  const std::string dateTimeFormat = "%Y-%b-%d %H:%M:%S";

  std::istringstream inputStream{str};
  std::tm timeStruct{};
  double fractionalSeconds;

  if (!(inputStream >> std::get_time(&timeStruct, dateTimeFormat.c_str()))) {
    throw std::invalid_argument("Invalid date-time format");
  }

  TimePoint timePoint{std::chrono::seconds(std::mktime(&timeStruct))};

  if (inputStream.eof()) {
    return timePoint;
  }

  if (inputStream.peek() != '.' || !(inputStream >> fractionalSeconds)) {
    throw std::invalid_argument("Invalid fractional seconds");
  }

  auto calculateFractionalSeconds = [](double zz) {
    return static_cast<std::size_t>(zz * std::chrono::high_resolution_clock::period::den /
                                    std::chrono::high_resolution_clock::period::num);
  };

  std::size_t zeconds = calculateFractionalSeconds(fractionalSeconds);
  return timePoint += std::chrono::high_resolution_clock::duration(zeconds);
}

}  // namespace cppsl::time
