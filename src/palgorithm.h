#ifndef PALGORITHM_H
#define PALGORITHM_H

#include <algorithm>

#ifdef USING_OPENMP
#include <parallel/algorithm>
#define PROCESS_MSG
#ifndef ALG_NS
#define ALG_NS __gnu_parallel
#endif

#endif

//fallback to std
#ifndef ALG_NS
#define ALG_NS std
#define PROCESS_MSG qApp->processEvents()
#endif


#endif // PALGORITHM_H
