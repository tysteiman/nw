#include "../lib.h"

#include "test.h"

void
fileExistsTest(file_t *file)
{
    /* fake path */
    int res = fileExists("foo.h");
    nw_assert(res, FALSE, "File: foo.h does not exist",
            __FILE__, __LINE__, (char *)__FUNCTION__, file);

    /* real path */
    int res2 = fileExists("nw.c");
    nw_assert(res2, TRUE, "File: nw.c exists",
            __FILE__, __LINE__, (char *)__FUNCTION__, file);            
}

void
stringEqTest  (file_t * file)
{
    char *mainStr = "tyler";

    char *wrongStr = "kyler";
    char *correctStr = "tyler";

    /* assert wrong */
    int res_wrong = stringEq(mainStr, wrongStr);

    nw_assert(res_wrong, FALSE, "kyler does not match tyler",
            __FILE__, __LINE__, (char *)__FUNCTION__, file);

    /* assert correct */
    int res_correct = stringEq(mainStr, correctStr);

    nw_assert(res_correct, TRUE, "tyler matches tyler",
            __FILE__, __LINE__, (char *)__FUNCTION__, file);
}