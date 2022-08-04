#include "getos.h"
#include <stdio.h>
#include <string.h>

void get_os_version()
{
    size_t len = 0;
    FILE *fp;
    char *str = NULL;

    fp = fopen(os_file, "r");
    if(NULL == fp){
    	printf("open %s failed\n", os_file);
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
