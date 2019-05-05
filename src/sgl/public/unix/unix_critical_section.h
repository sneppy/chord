#pragma once

#include "core_types.h"
#include "hal/pthread_critical_section.h"

/// Unix uses pthreads
typedef PThreadCriticalSection CriticalSection;
typedef PThreadRWLock RWLock;

