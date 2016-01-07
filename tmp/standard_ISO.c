#include <stdio.h>


int main()
{
    int RealISOValue;
    int StandardISOValue;

    scanf("%d",&RealISOValue);

    if(RealISOValue < 28)
    {
        StandardISOValue = 20;
    }else if(RealISOValue >=28 && RealISOValue < 36){
        StandardISOValue = 32;
    }else if(RealISOValue >=36 && RealISOValue < 45){
        StandardISOValue = 40;
    }else if(RealISOValue >= 45 && RealISOValue < 57){
        StandardISOValue = 50;
    }else if(RealISOValue >= 57 && RealISOValue < 71){
        StandardISOValue = 64;
    }else if(RealISOValue >= 71 && RealISOValue < 89){
        StandardISOValue = 80;
    }else if(RealISOValue >=89 && RealISOValue < 112){
        StandardISOValue = 100;
    }else if(RealISOValue >=112 && RealISOValue < 141){
        StandardISOValue= 125;
    }else if(RealISOValue >= 141 && RealISOValue < 178){
        StandardISOValue = 160;
    }else if(RealISOValue >= 178 && RealISOValue < 225){
        StandardISOValue = 200;
    }else if(RealISOValue >= 225 && RealISOValue < 283){
        StandardISOValue = 250;
    }else if(RealISOValue >= 283 && RealISOValue < 356){
        StandardISOValue = 320;
    }else if(RealISOValue >= 356 && RealISOValue < 449){
        StandardISOValue = 400;
    }else if(RealISOValue >= 449 && RealISOValue < 566){
        StandardISOValue = 500;
    }else if(RealISOValue >= 566 && RealISOValue < 713){
        StandardISOValue = 640;
    }else if(RealISOValue >= 713 && RealISOValue < 891){
        StandardISOValue = 800;
    }else if(RealISOValue >= 891 && RealISOValue < 1122){
        StandardISOValue = 1000;
    }else if(RealISOValue >= 1122 && RealISOValue < 1414){
        StandardISOValue = 1250;
    }else if(RealISOValue >= 1414 && RealISOValue < 1782){
        StandardISOValue = 1600;
    }else if(RealISOValue >= 1782 && RealISOValue < 2245){
        StandardISOValue = 2000;
    }else if(RealISOValue >= 2245 && RealISOValue < 2828){
        StandardISOValue = 2500;
    }else if(RealISOValue >= 2828 && RealISOValue < 3564){
        StandardISOValue = 3200;
    }else{
        StandardISOValue = 4000;
    }

    printf("RealISOValue=%d,StandardISOValue=%d\n",RealISOValue,StandardISOValue);

    return 1;
}
