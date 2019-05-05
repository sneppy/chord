#pragma once

/**
 * Sets value to true if type id POD
 */
template<typename T>
struct IsPod { enum {value = __is_pod(T)}; };