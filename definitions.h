#include<stdio.h>
#define MAX_MEM 4
#define MIN_MEM MAX_MEM/2
#define DIMENSIONS 2
#define EDGES 2*DIMENSIONS

int id=1;
struct rectangle{
    float edges[EDGES];
};
struct member{
    struct rectangle rect;
    struct node* child;
};
struct node{
    int no_entries;
    int level;
    struct member member[MAX_MEM];
};
struct insert_return_type{
    int n;
    struct member * upd_arr;
};

//defintions for split
struct member member_buffer[MAX_MEM+1];
int buffercount;
struct rectangle cover_split;
float cover_split_area;

struct crude_split_list{
    int split_list[MAX_MEM+1];
    int taken[MAX_MEM+1];
    int total;
    int min_mem;
    int count[2];
    struct rectangle seed_cover[2];
    float seed_area[2];
}s_list;

