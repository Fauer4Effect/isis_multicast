#include "gtest/gtest.h"
#include <boost/asio.hpp>

#include "messages.hpp"
#include "multicast.hpp"

// TODO so testing single multicaster for things like bad messages incoming
// etc is easy enough to do

// TODO multiple casters is harder, because of the way that we defined the 
// whole thing we expect to be sending out on the same port that we're
// listening on. 
// ??? Option to do whatever using docker, which we probably should do at some 
// point, but there's also the idea of using a mock. But is too much of the 
// functionality already baked into the main thing

// TODO some setup things, failed to bind port, multiple multi on same port


// TODO what happens if could not send multicast to a host
// What is cannot send single to a host
// Ideally they should just continue, that's kind of the whole point

// TODO other things like receives bad message
// wrong type
// wrong sender index
// etc
// we don't promise any support under byzantine conditions but that doesn't
// mean that it should crash our system.

// TODO what if we receive way more than just one message worth of data
// will we overflow the buffer?

// TODO Also we need to check that we don't throw any exceptions from the whole
// failure to serialize etc
// In general we want to be careful about throwing exceptions that we can 
// recover from because if they're not handled we'll break out of the 
// event loop


// ??? so here's a thought, do we really need to have multiple hosts
// when we are just doing the testing 
// because we can simulate multiple messages coming in by constructing our
// own messages
// so we can set up a multicaster with multiple entry host file and pretend
// like it is getting messages from those other hosts
// Obviously it's not perfect but throw in some docker tests and maybe 
// it's sufficient

// XXX I don't think that mocking will help here, 1) we don't have a interface
// with virtual methods to mock and 2) there's nothing that really gets passed
// to the functions beyond the initial multicast etc.
// Actually that may be worth mocking not really sure