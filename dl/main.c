#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv)
{
	void *handle;
	void (*test) (void);
	char *error;
	handle = dlopen("./test.so", RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(1);
	}
	test = dlsym(handle, "test");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(1);
	}
	(*test)();
	dlclose(handle);
	handle = dlopen("./test1.so", RTLD_LAZY);
	test = dlsym(handle, "test");
	(*test)();
	dlclose(handle);
}
