/*
 * Hack to satisfy MSVCRT dependency in UCRT environment.
 * Do NOT include stdio.h here to avoid conflict.
 */

#ifdef __cplusplus
extern "C" {
#endif

// Dummy buffer to act as FILE array
static char dummy_iob[256]; 

// Define __iob_func
void * __cdecl __iob_func(void)
{
    return dummy_iob;
}

// Define the import pointer that the linker is looking for
void * (*__imp___iob_func)(void) = &__iob_func;

#ifdef __cplusplus
}
#endif
