#ifndef AR_UTIL_INCLUDED
#define AR_UTIL_INCLUDED

#define AR_PI 3.14159265358979323846264338327950
#define AR_DBL_INF (1.0/0.0)

#define AR_STR(x) #x

#define AR_UNUSED(x) ((void)x)

#define AR_SWAP(T, a, b) do { T temp = a; a = b; b = temp; } while(0)

int ar_is_good_double(double x);
void ar_print_bad_double(double x, int line, const char *path);

#define AR_ASSERT_GOOD_NUMBER(x) do { if (!ar_is_good_double(x)) ar_print_bad_double(x, __LINE__, __FILE__); }while(0)

#endif