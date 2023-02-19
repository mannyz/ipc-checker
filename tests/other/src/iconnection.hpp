#pragma once

#include "interface/impl_traits.hpp"
#include "interface/interface.hpp"

#include "ichannel.hpp"
#include "tags.hpp"

INTERFACE(IConnection)
USING_IMPL_FOR_TAG(TChannelImpl, tags::Connection::Channel);
DECLARE_METHOD(IChannel<TChannelImpl>, GetChannel, ());
END_INTERFACE();
