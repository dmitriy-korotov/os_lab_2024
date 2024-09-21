#include "revert_string.h"

#include <swap.h>

#include <string.h>

void RevertString(char *str)
{
	size_t size = strlen(str);
	for (size_t i = 0; i < size / 2; i++) {
		Swap(str + i, str + size - i - 1);
	}
}

