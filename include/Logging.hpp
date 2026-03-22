
#pragma once

/**************************************************************
 * @file   Logging.hpp
 * @date   18.02.26
 * @author Julian Benzel
 * @brief  basic logging-utility.
 **************************************************************/

#include <iomanip>
#include <iostream>
#include <string_view>
#include <ctime>

#include <Types.hpp>

namespace ng
{

    /**************************************************************
     * @brief logging-methods. will return immediately if
     *        NGROUBIKS_SUPPRESS_LOGS is defined.
     **************************************************************/

    void log(std::string_view msg,
        std::string_view prefix = "",
        std::ostream& os = std::cout,
        bool timestamp = true,
        std::string_view datetimefmt = "%d-%m-%Y %H:%M:%S",
        std::string_view delim = " : "
    );

    void log_info(std::string_view msg);
    void log_debug(std::string_view msg);
    void log_error(std::string_view msg);
    void log_warning(std::string_view msg);
    
}

inline void ng::log(std::string_view msg,
    std::string_view prefix,
    std::ostream& os,
    bool timestamp,
    std::string_view datetimefmt,
    std::string_view delim)
{
#ifndef NGROUBIKS_SUPPRESS_LOGS
    /* print timestamp, if true */
    if (timestamp) {
        std::time_t time = std::time(nullptr);
        std::tm tm = *std::localtime(&time);
        os << std::put_time(&tm, datetimefmt.data()) << delim;
    }
    /* print prefix, if given */
    if (!prefix.empty()) {
        os << std::format("[{}]", prefix) << delim;
    }
    /* print actual message */
    os << msg << std::endl;
#endif
}

inline void ng::log_info(std::string_view msg) {
#ifndef NGROUBIKS_SUPPRESS_LOGS
    ng::log(msg, "INFO");
#endif
}

inline void ng::log_debug(std::string_view msg) {
#ifndef NGROUBIKS_SUPPRESS_LOGS
    ng::log(msg, "DEBUG");
#endif
}

inline void ng::log_error(std::string_view msg) {
#ifndef NGROUBIKS_SUPPRESS_LOGS
    ng::log(msg, "ERROR", std::cerr);
#endif
}

inline void ng::log_warning(std::string_view msg) {
#ifndef NGROUBIKS_SUPPRESS_LOGS
    ng::log(msg, "WARNING", std::cerr);
#endif  
}