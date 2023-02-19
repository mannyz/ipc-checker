#pragma once

#include "interface/generator/tags.hpp"
#include "interface/impl_traits.hpp"
#include "interface/interface.hpp"

INTERFACE(IGenerator)
USING_IMPL_FOR_TAG(TObject, tags::Generator::GeneratedObject);
DECLARE_METHOD(void, ToFirst, ());
DECLARE_METHOD(bool, HasNext, (), (const));
DECLARE_METHOD(TObject, Next, ());
END_INTERFACE();
