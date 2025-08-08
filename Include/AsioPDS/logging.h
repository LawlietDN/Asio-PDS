#pragma once
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

namespace pdslog
{

enum Level
{
             error = boost::log::trivial::error,
             warn  = boost::log::trivial::warning,
             info  = boost::log::trivial::info,
             debug = boost::log::trivial::debug,
             trace = boost::log::trivial::trace };

inline void init(Level minLevel = info)
{
    namespace lg = boost::log;
    lg::add_console_log(std::clog,
        lg::keywords::format =
            (lg::expressions::stream
                << "[" << lg::trivial::severity << "] "
                << lg::expressions::smessage));

    lg::core::get()->set_filter(
        lg::trivial::severity >=
        static_cast<lg::trivial::severity_level>(minLevel));
}

#define LOG(lvl) BOOST_LOG_TRIVIAL(lvl)

} 
