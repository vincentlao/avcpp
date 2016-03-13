#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <chrono>

#include "rational.h"

namespace av {

/**
 * @brief The Timestamp class represents timestamp value and it timebase
 */
class Timestamp
{
public:
    Timestamp() noexcept;
    Timestamp(int64_t timestamp, const Rational &timebase) noexcept;

    /**
     * @brief Create AvCpp/FFmpeg compatible timestamp value from the std::chrono::duration/boost::chrono::duration
     */
    template<typename Duration>
    Timestamp(const Duration& duration)
    {      
        using Ratio = typename Duration::period;

        static_assert(Ratio::num <= INT_MAX, "To prevent precision lost, ratio numerator must be less then INT_MAX");
        static_assert(Ratio::den <= INT_MAX, "To prevent precision lost, ratio denominator must be less then INT_MAX");

        m_timestamp = duration.count();
        m_timebase  = Rational(static_cast<int>(Ratio::num),
                               static_cast<int>(Ratio::den));
    }

    int64_t         timestamp() const noexcept;
    const Rational& timebase() const noexcept;

    operator bool() const noexcept;
    bool isValid() const noexcept;

    operator double() const noexcept;
    double seconds() const noexcept;

    /**
     * @brief Convert to the std::chrono::duration compatible value
     */
    template<typename Duration>
    Duration toDuration() const
    {
        using Ratio = typename Duration::period;

        static_assert(Ratio::num <= INT_MAX, "To prevent precision lost, ratio numerator must be less then INT_MAX");
        static_assert(Ratio::den <= INT_MAX, "To prevent precision lost, ratio denominator must be less then INT_MAX");

        Rational dstTimebase(static_cast<int>(Ratio::num),
                             static_cast<int>(Ratio::den));
        auto ts = m_timebase.rescale(m_timestamp, dstTimebase);
        return Duration(ts);
    }

private:
    int64_t  m_timestamp = AV_NOPTS_VALUE;
    Rational m_timebase = AV_TIME_BASE_Q;
};


//
// Comparation
//

inline
bool operator>(const Timestamp& left, const Timestamp &right) noexcept
{
    return av_compare_ts(left.timestamp(), left.timebase(),
                         right.timestamp(), right.timebase()) > 0;
}


inline
bool operator<(const Timestamp& left, const Timestamp &right) noexcept
{
    return av_compare_ts(left.timestamp(), left.timebase(),
                         right.timestamp(), right.timebase()) < 0;
}


inline
bool operator==(const Timestamp& left, const Timestamp &right) noexcept
{
    return av_compare_ts(left.timestamp(), left.timebase(),
                         right.timestamp(), right.timebase()) == 0;
}

inline
bool operator!=(const Timestamp& left, const Timestamp &right) noexcept
{
    return !operator==(left, right);
}


//
// Math operations
//

Timestamp operator+(const Timestamp& left, const Timestamp &right) noexcept;
Timestamp operator-(const Timestamp& left, const Timestamp &right) noexcept;
Timestamp operator*(const Timestamp& left, const Timestamp &right) noexcept;
Timestamp operator/(const Timestamp& left, const Timestamp &right) noexcept;

//
// Output
//
std::ostream& operator<<(std::ostream &ost, const Timestamp &ts)
{
    ost << ts.timestamp() << '*' << ts.timebase();
    return ost;
}

} // ::av

#endif // TIMESTAMP_H
