#include <stdio.h>
#include <string.h>
#include <math.h>
 
void tostring(char [], int);
int toint(char []);
 
int main()
{
    char str[5];
    int num, result, result1;
 
    printf("Enter a number: ");
    scanf("%d", &num);
    //tostring(str, num);
    result = intToBinary(num);
    printf("Number converted to binary: %d\n", result);
    tostring(str,result);
    printf("filters: %s\n", str);
 
    return 0;
}
 
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
    char filters[5];
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        filters[len - (i + 1)] = rem + '0';
    }
    filters[len] = '\0';


                    if(strlen(filters)<4)
                    {
                        int t2=4-strlen(filters);
                        int t3,t4=0;
                        for(t3=0;t3<t2;t3++)
                            str[t3]='0';
                        for(;t3<4;t3++)
                        {    
                            str[t3]=filters[t4];
                            t4++;
                        }    
                        str[t3]='\0';
                    }  
                    else
                        strcpy(str,filters);  
}

int intToBinary(int num)
{
    int remainder, base = 1, binary = 0;
    
    while (num > 0)
    {
        remainder = num % 2;
        binary = binary + remainder * base;
        num = num / 2;
        base = base * 10;
    }
    return binary;
}