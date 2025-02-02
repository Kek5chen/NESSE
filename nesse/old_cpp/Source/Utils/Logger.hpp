#pragma once

#include <iostream>

// Debug
#if defined(DEBUG) || defined (_DEBUG)
# define DLOG(x) std::cout << "[Debug] " << x << std::endl
#else
# define DLOG(...)
#endif

// Info
#define ILOG(x) std::cout << "[Info] " << x << std::endl

// Warning
#define WLOG(x) std::cout << "[Warn] " << x << std::endl

// Error
#define ELOG(x) std::cout << "[Error] " << x << std::endl