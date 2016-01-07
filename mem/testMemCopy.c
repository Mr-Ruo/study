#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct front{
    int frnt; //1 or 0
};

struct face
{
    int rect[2];
    struct front *pFront;
};



int main()
{
    
    struct front ft={0};

    struct face mFace;
    
    mFace.rect[0]=1;
    mFace.rect[1]=2;

    mFace.pFront=&ft;


    struct face* ptr= (struct face*)malloc(sizeof(struct face));


    ptr=(struct face*)memcpy((void *)ptr,(void *)&mFace,sizeof(struct face));

    ptr->pFront->frnt=100;
    printf("%d,%d,%p,%p,%d\n",ptr->rect[0],ptr->pFront->frnt,&ft,ptr->pFront,ft.frnt);

    return 1;

}

