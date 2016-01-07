#include <stdio.h>

void move(int position);

int move_lens(int g_u4CurrentPosition)
{
    int cnt,targetPosition;
    int af_nature_position = 50;
    targetPosition = g_u4CurrentPosition;

    for(cnt=2;targetPosition >= af_nature_position;cnt++){
        targetPosition -= targetPosition/cnt;
        move(targetPosition);
        targetPosition +=10;
        move(targetPosition);
    }

    return 0;
}

void move(int position)
{
    printf("move to position:%d\n",position);
}


int main()
{
    int g_u4CurrentPosition;
    scanf("%d",&g_u4CurrentPosition);

    move_lens(g_u4CurrentPosition);
}
