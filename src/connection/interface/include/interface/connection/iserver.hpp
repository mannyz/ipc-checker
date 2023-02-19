#pragma once

#include "interface/connection/ibuffer.hpp"

INTERFACE(IServer)
DECLARE_TEMPLATE_METHOD((TBufferImpl), void, Receive,
                        (IBuffer<TBufferImpl> & buffer));
END_INTERFACE();