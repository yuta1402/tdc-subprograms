#ifndef MEASURE_TIME_HPP
#define MEASURE_TIME_HPP

#include <iostream>
#include <chrono>
#include <string>
#include <unordered_map>
#include "stopwatch.hpp"

/*!
@namespace estd
@brief extended std
*/
namespace estd
{
    class measure_time
    {
    public:
        measure_time(const std::string& event_name) :
            event_name_{ event_name },
            stopwatch_{ true }
        {
        }

        ~measure_time()
        {
            auto elapsed = stopwatch_.elapsed();
            std::cout << event_name_ << ": " << elapsed << "[ms]" << std::endl;
        }

    private:
        const std::string event_name_;
        estd::stopwatch stopwatch_;
    };

    class measure_time_for
    {
    public:
        measure_time_for(const std::string& main_event_name) :
            main_event_name_{ main_event_name }
        {
        }

        void start(const std::string& sub_event_name)
        {
            stopwatches_[sub_event_name].start();
        }

        void pause(const std::string& sub_event_name)
        {
            stopwatches_[sub_event_name].pause();
        }

        ~measure_time_for()
        {
            std::cout << main_event_name_ << ":" << std::endl;

            for (const auto& s : stopwatches_) {
                std::cout << "    " << s.first << ": " << s.second.elapsed() << "[ms]" << std::endl;
            }
        }

    private:
        const std::string main_event_name_;
        std::unordered_map<std::string, stopwatch> stopwatches_;
    };
}

#endif
