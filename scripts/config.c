#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


void main(int argc, char *argv[]) {
    char *host=malloc(256);
    int n = argc;
    for (int i=1; i < n; i++) {
	host[i] = argv[n+1];
	printf(host);
    }
}