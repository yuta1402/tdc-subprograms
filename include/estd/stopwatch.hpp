#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <chrono>
#include <iostream>

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    class stopwatch
    {
    public:
        using time_point = std::chrono::system_clock::time_point;

    public:
        stopwatch(bool start_immediately = true)
        {
            if (start_immediately) {
                start();
            }
        }

        ~stopwatch() = default;

        void start()
        {
            pausing_ = false;

            start_time_ = std::chrono::system_clock::now();
        }

        void pause()
        {
            accumulation_us_ = us();

            pausing_ = true;
        }

        void reset()
        {
            accumulation_us_ = 0;

            pausing_ = true;
        }

        void restart()
        {
            reset();

            start();
        }

        std::int64_t us() const
        {
            auto end_time = std::chrono::system_clock::now();

            if (pausing_) {
                return accumulation_us_;
            }

            auto t = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
            return accumulation_us_ + t.count();
        }

        double usf() const { return static_cast<double>(us()); }

        std::int64_t ms() const { return us() / 1000LL; }
        std::int64_t s() const { return us() / (1000LL * 1000LL); }
        std::int64_t elapsed() const { return ms(); }

        double msf() const { return static_cast<double>(us()) / static_cast<double>(1000LL); }
        double sf() const { return static_cast<double>(us()) / static_cast<double>(1000LL * 1000LL); }
        double elapsedf() const { return msf(); }

    private:
        time_point start_time_;

        bool pausing_{ true };
        std::int64_t accumulation_us_{ 0 };
    };
}

#endif
