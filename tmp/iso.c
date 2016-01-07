/*
 *Date:2015/12/18
 *Author:396527290@qq.com
 *Function:Get the Standard ISO Value
 *Description:Just a demo,use the Table-Driven Approach to reduce the searching time.
 * */

#include <stdio.h>

#define LIMIT_LENGTH 26

static ISOStandardTable[]={
    25,32,40,50,64,80,100,
    125,160,200,250,320,400,
    500,640,800,1000,1250,1600,
    2000,2500,3200,4000,5000,6400,8000
};

static ISOLimitTable[]={
    28,36,45,57,71,89,112,
    141,178,225,283,356,449,566,
    713,891,1122,1414,1782,2245,
    2828,3564,4490,5657,7127,8909
};


int BinarySearch(int ISOValue)
{
    int low = 0;
    int high = LIMIT_LENGTH-1;
    int mid = 0;

    while(low <= high)
    {
        mid = (low + high)/2;

        if(ISOValue > ISOLimitTable[mid]){
            low = mid + 1;
        }else if(ISOValue < ISOLimitTable[mid]){
            high = mid - 1;
        }else{ 
            return mid;
        }
    }

    return low;
}

int main()
{
    int RealISOValue,StandardISOIndex;

    scanf("%d",&RealISOValue);

    StandardISOIndex=BinarySearch(RealISOValue);

    printf("StandardISOValue=%d,StandardISOIndex=%d\n",ISOStandardTable[StandardISOIndex],StandardISOIndex);

    return 1;
}
