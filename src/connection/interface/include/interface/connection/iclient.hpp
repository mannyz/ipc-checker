#pragma once

#include "interface/connection/ibuffer.hpp"

INTERFACE(IClient)
DECLARE_TEMPLATE_METHOD((TBufferImpl), void, Send,
                        (const IBuffer<TBufferImpl> &buffer));
END_INTERFACE();
