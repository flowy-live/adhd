#include <sqlite3.h>
#include "common.h"

/* persist on disk with compact representation
    date representation -> value
 */
void store_result(U32 val, struct date d);
void retrieve_result(U32 val, struct date d);
void retrieve_results_before(int *vals, struct date before);

