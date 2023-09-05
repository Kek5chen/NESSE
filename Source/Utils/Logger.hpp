#pragma once

#include <iostream>

// Debug
#if defined(DEBUG) || defined (_DEBUG)
# define DLOG(x) std::cout << "[D] " << x << std::endl
#else
# define DLOG(...)
#endif

// Info
#define ILOG(x) std::cout << "[I] " << x << std::endl

// Warning
#define WLOG(x) std::cout << "[W] " << x << std::endl

// Error
#define ELOG(x) std::cout << "[E] " << x << std::endl