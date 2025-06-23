#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


int main( int argc, char *argv[]) {

	char sln[100];
	char *pt;
	int a;
	printf("%s\n", argv[1]);

	pt = realpath (argv[1], sln);

		printf("%s\n",pt);

		unlink(argv[1]);

		a = symlink(pt, argv[1]);

		printf("%d\n", a);

	return 0;

}
