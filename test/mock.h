#ifndef __MOCK_H__
#define __MOCK_H__

#include "../src/adf.h"

series_t get_series(void);
series_t get_repeated_series(void);
adf_t get_default_object(void);
adf_t get_object_with_zero_series(void);

#endif /* __MOCK_H__ */
