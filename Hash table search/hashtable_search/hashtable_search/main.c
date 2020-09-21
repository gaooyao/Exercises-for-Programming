#define _CRT_SECURE_NO_WARNINGS

#include <time.h>
#include <stdio.h>
// #include <Windows.h>

// SDBMHashÀ„∑®
unsigned int SDBMHash(char* str) {
    unsigned int hash = 0;
    while (*str)
    {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }
    return (hash & 0x7FFFFFFF);
}


// RS HashÀ„∑®
unsigned int RSHash(char* str) {
    unsigned int b = 378551;
    unsigned int a = 63689;
    unsigned int hash = 0;
    while (*str)
    {
        hash = hash * a + (*str++);
        a *= b;
    }
    return (hash & 0x7FFFFFFF);
}


// JS HashÀ„∑®
unsigned int JSHash(char* str) {
    unsigned int hash = 1315423911;
    while (*str)
    {
        hash ^= ((hash << 5) + (*str++) + (hash >> 2));
    }
    return (hash & 0x7FFFFFFF);
}



// P.J.Weinberger HashÀ„∑®
unsigned int PJWHash(char* str) {
    unsigned int BitsInUnignedInt = (unsigned int)(sizeof(unsigned int) * 8);
    unsigned int ThreeQuarters = (unsigned int)((BitsInUnignedInt * 3) / 4);
    unsigned int OneEighth = (unsigned int)(BitsInUnignedInt / 8);
    unsigned int HighBits = (unsigned int)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
    unsigned int hash = 0;
    unsigned int test = 0;
    while (*str)
    {
        hash = (hash << OneEighth) + (*str++);
        if ((test = hash & HighBits) != 0)
        {
            hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
        }
    }
    return (hash & 0x7FFFFFFF);
}



// ELF HashÀ„∑®
unsigned int ELFHash(char* str) {
    unsigned int hash = 0;
    unsigned int x = 0;
    while (*str)
    {
        hash = (hash << 4) + (*str++);
        if ((x = hash & 0xF0000000L) != 0)
        {
            hash ^= (x >> 24);             hash &= ~x;
        }
    }
    return (hash & 0x7FFFFFFF);
}

// BKDR HashÀ„∑®
unsigned int BKDRHash(char* str) {
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}



// DJB HashÀ„∑®
unsigned int DJBHash(char* str) {
    unsigned int hash = 5381;
    while (*str)
    {
        hash += (hash << 5) + (*str++);
    }
    return (hash & 0x7FFFFFFF);
}



// AP HashÀ„∑®
unsigned int APHash(char* str) {
    unsigned int hash = 0;
    int i;
    for (i = 0; *str; i++)
    {
        if ((i & 1) == 0)
        {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        }
        else
        {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash & 0x7FFFFFFF);
}

// using namespace std;



int main() {
    //start timer
    for(int a=0;a<50;)
    {
       
        clock_t start_t, end_t;
        //double total_t;
        //int i;
        // start_t = clock();/Users/yangen/Desktop/Exercises-for-Programming/Hash table search/hashtable_search
        // printf("程序启动，start_t = %ld\n", start_t);
        // printf("开始一个大循环，start_t = %ld\n", start_t);
        
        
            FILE* f_string = NULL;
            f_string = fopen("string.txt", "wr");
            //start_t = clock();
            if (!f_string)
            {
                printf("Failed\n");//读文件失败
                return 0;
            }
            char list[10000][100];
            double total_time = 0;
            for(int j = 0;j<200;j++){
                for(int k=0;k<10000;k++){
                    fscanf(f_string, "%s", list[k]);//xieru+duqu
                }
                start_t = clock();
                //SDBMHash,RSHash,JSHash,PJWHash,ELFHash,BKDRHash,DJBHash,APHash
                for(int k=0;k<10000;k++){
                    //printf("%s",list[k]);
                    APHash(list[k]);
                }
                end_t=clock();
                total_time+=(double)(end_t-start_t);
                
            }
            printf(" %f\n",total_time/ CLOCKS_PER_SEC);
            //a=a+1;
            //printf("a=%d\n",a);
            //return 0;
            /*
             for (size_t i = 0; i < 200000; i++)
             {
             char str[99];
             fscanf(f_string, "%s", str);
             //SDBMHash RSHash JSHash PJWHash ELFHash BKDRHash DJBHash APHash
             // printf("%12s 的hash值是：%u\n", str,APHash(str));
             APHash(str);
             }
             fclose(f_string);
             // conclude timer
             end_t = clock();
             printf("大循环结束，end_t = %ld\n", end_t);
             total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
             printf("CPU 占用的总时间：%f\n", total_t  );
             printf("程序退出...\n");
             return(0);
             */
        }
    }

