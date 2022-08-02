#include "getos.h"
#include <stdio.h>
#include <string.h>

#define os_release "/etc/os-release"

void get_os_version()
{
    size_t len = 0;
    FILE *fp;
    char *str = NULL;

    fp = fopen(os_release, "r");
    if(NULL == fp){
    	printf("open %s failed\n", os_release);
	return;
    }

    while(getline(&str, &len, fp) != -1){
    	if(0 == strncmp(str, "VERSION=", 8)){
    	    printf("%s", str);
            break;
        }
    }

    fclose(fp);
    return;
}
