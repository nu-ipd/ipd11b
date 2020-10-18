#include <stdlib.h>

int* allocate_it(size_t n)
{
    return malloc(n * sizeof(int));
}

void use_it(size_t i, int* p)
{
    ++p[i];
}

int main(void)
{
    use_it(6, allocate_it(5));
}
