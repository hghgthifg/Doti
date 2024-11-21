module;
#include <boost/signals2/signal.hpp>

export module Boost.Signals2;

export namespace Boost::Signals2
{
    using boost::signals2::signal;
    using boost::signals2::connection;
    using boost::signals2::scoped_connection;
}
