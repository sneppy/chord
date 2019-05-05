#pragma once

#include "core_types.h"
#include <memory>

/**
 * Just a typedef for now
 * I may implement this from scratch
 * keeping the same interface
 */
template<typename T>
using UniquePtr = std::unique_ptr<T>;

