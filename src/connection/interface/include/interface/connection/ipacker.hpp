#pragma once

#include "interface/connection/ibuffer.hpp"

INTERFACE(IPacker)
DECLARE_TEMPLATE_METHOD((TBufferImpl, TObject), void, Pack,
                        (IBuffer<TBufferImpl> & buffer, const TObject &object));
// TODO:  add universal reference support in DECLARE api,
//        so that next declaration would be available:
//          DECLARE_TEMPLATE_METHOD((TBufferImpl, TObject),
//                                  void, Pack,
//                                  (IBuffer<TBufferImpl> & buffer,
//                                  TObject&& object));
DECLARE_TEMPLATE_METHOD((TBufferImpl, TObject), void, Unpack,
                        (IBuffer<TBufferImpl> & buffer, TObject &object));
END_INTERFACE();