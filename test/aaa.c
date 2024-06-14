#include <stdlib.h>
#include <stdio.h>

typedef struct {
	int n;
	int *val;
} a;

int main(){
	int *vals = malloc(sizeof(int)*2);
	*(vals) = 123;
	*(vals + 1) = -234;

	a f = {
		.n = -123890,
		.val = vals
	};

	printf("[f] n:%d, val[0]:%d, val[q]:%d\n", f.n, f.val[0], f.val[1]);
	a s = f;
	s.n = 13;
	*(s.val) = 333333;
	printf("[f] n:%d, val[0]:%d, val[q]:%d\n", f.n, f.val[0], f.val[1]);
	printf("[s] n:%d, val[0]:%d, val[q]:%d\n", s.n, s.val[0], s.val[1]);
}