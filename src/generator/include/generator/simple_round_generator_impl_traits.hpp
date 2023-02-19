#pragma once

#include "interface/generator/tags.hpp"
#include "interface/impl_traits.hpp"

class SimpleRoundGenerator;

IMPL_TRAITS_DEFINITION(SimpleRoundGenerator)
USE_IMPL_FOR_TAG(tags::Generator::GeneratedObject, std::string);
END_IMPL_TRAITS_DEFINITION();
