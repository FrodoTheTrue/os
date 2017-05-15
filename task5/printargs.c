#include <stdio.h>
#include <syslog.h>

int main(int argc, char * argv[]) {
	int i;	
	for (i = 0; i < argc; i++) {
		syslog(LOG_NOTICE, "%s\n", argv[i]);
	}
}