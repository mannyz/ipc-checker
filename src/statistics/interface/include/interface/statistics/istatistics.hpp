#pragma once

#include "interface/measurer/imeasurer.hpp"

INTERFACE(IStatistics)
DECLARE_TEMPLATE_METHOD((TProducerMeasurerImpl, TConsumerMeasurerImpl), void,
                        Process,
                        (IMeasurer<TProducerMeasurerImpl> &,
                         IMeasurer<TConsumerMeasurerImpl> &));
END_INTERFACE();
