#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define LENGHT ((int)(pow(10,8)))
#define WRITE_PACK 4096
#define data_size (WRITE_PACK+8+1)
char data[data_size];
uint64_t data_use;

FILE *write_ptr;

uint8_t is_inside_data2[100000000];

bool not_is_inside2(uint64_t i){
    if(is_inside_data2[i])
        return false;
    else
        is_inside_data2[i] = 1;
    return true;
}

uint64_t fusion(uint64_t old,uint64_t new_int, uint8_t round){
    return (uint64_t)(old * (uint64_t)pow(10,round +1) + new_int) % LENGHT;
}

void add_data(uint64_t in, uint8_t round){
    const char lookup[]={'0','1','2','3','4','5','6','7','8','9'};
    int8_t internal_round = round;
    char *tmp = malloc(round+1);
    for(int i = 0; i<=round;i++){
        tmp[i] = in % 10;
        in /= 10;
    }   
    while(internal_round + 1){
        data[data_use++] = lookup[ tmp[internal_round--]];
    }
    //data[data_use++] = ' ';
    if(data_use >= WRITE_PACK){
        printf("write..%i..",WRITE_PACK);
        fwrite(data,WRITE_PACK,sizeof(char),write_ptr); // write 10 bytes from our buffer
        data_use -= WRITE_PACK;
        for(int i = 0; i<=data_use;i++){
            data[i] = data[i+WRITE_PACK];
        }
        printf("done\n");
    }
    free(tmp);
}

void makeNew(volatile uint64_t* in){
    for(uint8_t x = 0; x < 8; x++){
        for(uint64_t y = 0,z = pow(10,x+1); y < z; y++)
            if(not_is_inside2(fusion(*in,y,x))){
                *in = fusion(*in,y,x);
                add_data(y,x);
                return;
            }
    }
    printf("damaged cycle %llu\n",*in);
    *in=*in+1%LENGHT;
}

void create_data(){
    memset(data,0,data_size);
    volatile uint64_t tmp = 0;
    add_data(tmp,7);
    not_is_inside2(0);
    for(uint64_t i = LENGHT-1; i; i--){
        makeNew(&tmp);
        //add_data(i,7);
    }
    printf("write..%i..",data_use);
    fwrite(data,data_use,sizeof(char),write_ptr);
    printf("done!\n");
}

void init(){
    memset(is_inside_data2,0,100000000);
}

int main(){
    printf("init\n");
    //printf("fusion test %i\n", fusion(12345678,321,2));
    init();
    printf("creating code.txt\n");
    write_ptr = fopen("code.txt","wb");  // w for write, b for binary
    printf("creating data...\n");
    create_data();
    for(uint64_t i = 0; i<LENGHT;i++)
        if(!is_inside_data2)
            printf("fogotten %llu\n");
    fclose(write_ptr);
}