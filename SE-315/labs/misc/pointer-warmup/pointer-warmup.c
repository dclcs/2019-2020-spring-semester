#include <stdio.h>
#include <stdlib.h>

void foo()
{
	printf("sizeof long: %lu\n", sizeof(long));
	long a[4];
	long *b = malloc(16);
	long *c;

	// 1. print address
	printf("1: a = %p, b = %p, c = %p\n", a, b, c);
	c = a;

	// 2. What's the mean of `c = a`?
	for (int i = 0; i < 4; i++)
		a[i] = 100 + i;
	c[0] = 200;
	printf("2: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);

	// 3. the usage of []
	c[1] = 300;
	*(c + 2) = 301;
	3 [c] = 302;
	printf("3: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);

	// 4. the meaning pointer add
	c = c + 1;
	*c = 400;
	printf("4: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);

	// 5. the meaning pointer add
	c = (long *)((char *)c + 1);
	*c = 500;
	printf("5: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);

	// 6. the meaning pointer add
	b = (long *)a + 1;
	c = (long *)((char *)a + 1);
	printf("6: a = %p, b = %p, c = %p\n", a, b, c);
}

int main()
{
	printf(">>> An basic pointer exercies for ChCore.\n");
	printf(">>> Please ensure you understand these output :)\n");

	foo();
	return 0;
}
