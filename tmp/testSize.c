#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


int main()
{
    printf("type\t\t\t\tsize\n");
    printf("char\t\t\t\t%d\n",sizeof(char));
    printf("short\t\t\t\t%d\n",sizeof(short));
    printf("int\t\t\t\t%d\n",sizeof(int));
    printf("long\t\t\t\t%d\n",sizeof(long));
    printf("unsigned int\t\t\t\t%d\n",sizeof(unsigned int));
    printf("unsigned long\t\t\t\t%d\n",sizeof(unsigned long));
    return 1;
}
