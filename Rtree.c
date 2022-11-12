#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <assert.h>
#include "definitions.h"

/*Function protoypes */
//general functions
void intialise_rect(struct rectangle *rectptr); // function to initialize a rectangle
struct node *getnode(); // function to give initial values to a function
void intialise_node(struct node *n); //function to initialize node
struct node *New_RTree(); // function to make a root node of r-tree
int check_rectangle(struct rectangle *rect); //function to check if the the coordinates of rectangle make a valid rectangle
float find_area(struct rectangle rect); // function to find area of given rectangle
int find_member(struct node *temp, struct rectangle *rect); // function to find appropriate member of a node in which we have to gon in
struct rectangle find_temparory_mbr(struct rectangle rect, struct rectangle rect2); // to find the temparory mbr of a rectangle
struct rectangle find_mbr(struct member *members, int entries); // to find the final mbr of combination of rectangles
float max(float a, float b); // to find the maximum of two float numbers
float min(float a, float b);// to find the minimum of two float number
// insertion
void insertion(struct node **root, struct rectangle *rect); //main insertion function used to insert node in r tree
struct insert_return_type insert2(struct node *in_node, struct rectangle *rect); // function to insert node in internal node
// splitting node
struct member *split_Quadratic(struct node *node, struct member mem); //main function of quadratic splitting
void Rtree_get_branches(struct node *n, struct member *m); // store all the members of node before splitting 
void split_grouping(struct crude_split_list *s, int min_mem); // allocate rectangles to the picked seeds
void Rtree_intialse_split_list(struct crude_split_list *s, int total, int min_mem); // function to initialize split lists
void Rtree_pick_seeds(struct crude_split_list *s);  // function to pick two seed rectangles whom we will group to form two new nodes
void Rtree_add_to_group(int i, int group, struct crude_split_list *s); // function to a rectangle to selected seed group
struct member *load_node(struct node *n1, struct node *n2, struct crude_split_list *s); //function to insert formed roup in a node
void add_member(struct member *mem, struct node *n); //function a member in a node
/*search*/
typedef int (*SearchHitCallback)(int id); // pointer to a function
int RTreeSearch(struct node *N, struct rectangle *R, SearchHitCallback shcb); // function to search the rectangles which overlap with the given rectangle
void RTreeRandomRect(struct rectangle *R); // function to create random rectangles
void RTreePrintRect(struct rectangle *R, int depth); // function to print rectangle if needed
int RTreeOverlap(struct rectangle *R, struct rectangle *S); // function to see if given two rectangles are overlapped or not
void RTreeTabIn(int depth); // functiion to just create proper spacing
/* functions */

// start
struct rectangle rects[100]; // an array of rectangles of a perticular size
int nrects = sizeof(rects) / sizeof(rects[0]); // give size of rectangle array
struct rectangle search_rect; // rectangle which is to be searched

int MySearchCallback(int id) 
{
    // Note: -1 to make up for the +1 when data was inserted
    printf("Hit data rectangle ID is %d\n", id - 1); // prints Id of hitted rectangle
    return 1; // keep going
}
struct rectangle take_input(float * val,int ext){
    struct rectangle temp;
    for(int i=0;i<DIMENSIONS;i++){
        temp.edges[i]=val[i]-ext;
        temp.edges[i+DIMENSIONS]=val[i]+ext;
    }
    return temp;
}
int main()
{   int ch;
    float coord[DIMENSIONS];
    struct rectangle temp_rect;
    int ext;
    for (int i = 0; i < 100; i++)
    {
        RTreeRandomRect(&rects[i]); // creating rectangle
    }
    search_rect = rects[20]; // setting rectangle which is to be searched
    struct node *root = New_RTree(); // creating root node
    int i, nhits;
    printf("nrects = %d\n", nrects);
    for (i = 0; i < nrects; i++)
        insertion(&root, &rects[i]); // inserting rectangles
    printf("100 values inserted\n");
    nhits = RTreeSearch(root, &search_rect, MySearchCallback);
    printf("Search resulted in %d hits\n", nhits);
    printf("searched rectangle:\n");
    for(int i=0;i<DIMENSIONS;i++){
        printf("Value of lower edge in DIMENSION %d :%f\n",i+1,search_rect.edges[i]);
        printf("Value of higher edge in DIMENSION %d :%f\n",i+1,search_rect.edges[i+DIMENSIONS]);
    }
    while(1){
        printf("**RTree Implementation**\n");
        printf("1. Insert\n");
        printf("2. Search near elemnts\n");
        printf("3.Quit\n");
        printf("Enter your choice :");
        scanf("%d",&ch);
        if(ch==1){
            for(int i=0;i<(DIMENSIONS);i++){
                printf("Enter the lower edge of %d Dimension :",i+1);
                scanf("%f",&temp_rect.edges[i]);
                printf("Enter the higher edge of %d Dimension :",i+1);
                scanf("%f",&temp_rect.edges[i+DIMENSIONS]);
            }
            insertion(&root,&temp_rect);
        }
        else if(ch==2){
            for(int i=0;i<DIMENSIONS;i++){
                printf("Enter the coordinate of %d Dimension :",i+1);
                scanf("%f",&coord[i]);
            }
            printf("Enter the range :");
            scanf("%d",&ext);
            temp_rect=take_input(coord,ext);
             nhits = RTreeSearch(root, &search_rect, MySearchCallback);
            printf("Search resulted in %d hits\n", nhits);
        }
        else if(ch==3){
            break;
        }

    }
    return 0;
}
// end
struct node *New_RTree()
{
    struct node *temp;
    temp = getnode();
    temp->level = 0;// setting level to leaf
    temp->no_entries = 0; // intitally no entry
    return temp;
}
struct node *getnode()
{
    struct node *temp = (struct node *)malloc(sizeof(struct node));// allocating memory
    temp->no_entries = 0; // no entry initially
    temp->level = -1; // to detect node is not proccesed 
    for (int i = 0; i < MAX_MEM; i++)
    {
        intialise_rect(&(temp->member[i].rect)); // forming null rectangles
        temp->member[i].child = NULL;
    }
    return temp;
}
void intialise_node(struct node *n)
{
    n->no_entries = 0;// no entry initially
    n->level = -1;// to detect node is not proccesed 
    for (int i = 0; i < MAX_MEM; i++)
    {
        intialise_rect(&(n->member[i].rect));// forming null rectangles
        n->member[i].child = NULL;
    }
}
void intialise_rect(struct rectangle *rectptr)
{
    for (int i = 0; i < EDGES; i++)
    {
        rectptr->edges[i] = (float)0;// initalizing rectangle to zero
    }
}
int check_rectangle(struct rectangle *rect)
{
    int flag = 1;
    for (int i = 0; i < DIMENSIONS; i++)
    {
        if (rect->edges[i] >= rect->edges[DIMENSIONS + i]) // checking if first coordinate is greater than other coordinate
        {
            flag = 0;
            break;
        }
    }
    return flag;
}
float find_area(struct rectangle rect)
{

    float prod = (rect.edges[0 + DIMENSIONS] - rect.edges[0]); // intializing prod with edge length of a rectangle
    for (int i = 1; i < DIMENSIONS; i++)
    {
        float ydist = (rect.edges[i + DIMENSIONS] - rect.edges[i]); // and updating with multipying with other dimension of rectangle
        prod *= ydist;// and updating with multipying with other dimension of rectangle
    }
    return prod;
}
int find_member(struct node *temp, struct rectangle *rect)
{
    float min_area = find_area(temp->member[0].rect); // finding area of given rectangle
    struct rectangle extended_mbr = find_temparory_mbr(temp->member[0].rect, *rect); // extending mbr
    float min_extended_area = find_area(extended_mbr) - find_area(temp->member[0].rect); // minimum required area to contain both rectangle
    float extended_area, area;
    int itr = 0;
    ;
    for (int i = 1; i < temp->no_entries; i++)
    {
        area = find_area(temp->member[i].rect);// area of given rectangle
        extended_mbr = find_temparory_mbr(temp->member[i].rect, *rect); // finding temporary 
        extended_area = find_area(extended_mbr) - area; //extended area
        if (extended_area < min_extended_area)// if extended area is less than minimum area than we set min area
        {
            min_extended_area = extended_area;
            min_area = area;
            itr = i;
        }
        else if (extended_area == min_extended_area) // if it is equal than check the rectangle with smaller original area
        {
            if (area < min_area)
            {
                min_extended_area = extended_area;
                min_area = area;
                itr = i;
            }
        }
    }
    return itr;
}
// function for insertions
void insertion(struct node **root, struct rectangle *rect)
{
    if (check_rectangle(rect)) // checking vallid rectangle
    {
        struct insert_return_type result;
        int level = (*root)->level;
        if ((*root)->level == 0) // if leaf
        {
            struct member *temp = (struct member *)malloc(sizeof(struct member));
            if ((*root)->no_entries == MAX_MEM)// full root split
            {
                temp->rect = *rect;
                temp->child = (struct node *)(++id); /* storing id*/
                struct member *member_array;
                member_array = split_Quadratic(*root, *temp);
                struct node *new_root = getnode();
                // creating new root
                new_root->level = level + 1;
                new_root->no_entries = 2;
                new_root->member[0] = member_array[0];
                new_root->member[1] = member_array[1];
                *root = new_root; // root updated
            }
            else // if root has space jsut insert
            {
                ((*root)->member[(*root)->no_entries]).rect = *rect;
                ((*root)->member[(*root)->no_entries]).child = (struct node *)id++;
                (*root)->no_entries++;
            }
        }
        else // if internal node
        {
            int n = find_member(*root, rect); // finding appropriate member
            struct insert_return_type temp = insert2((*root)->member[n].child, rect); // calling inserrt 2 to insert in internal node
            if (temp.n == 1) // if no split in child
            {
                (*root)->member[n] = temp.upd_arr[0];
            }
            else if (temp.n == 2) // if split in child
            {
                if ((*root)->no_entries == MAX_MEM) // if node is full
                {
                    (*root)->member[n] = temp.upd_arr[0];
                    struct node *new_root = getnode();
                    struct member *member_array;
                    new_root->level = level + 1;
                    new_root->no_entries = 2;
                    // creating new root
                    member_array = split_Quadratic(*root, temp.upd_arr[1]);
                    new_root->member[0] = member_array[0];
                    new_root->member[1] = member_array[1];

                    *root = new_root; // updated root
                }
                else
                {
                    (*root)->member[n] = temp.upd_arr[0];
                    (*root)->member[(*root)->no_entries] = temp.upd_arr[1];
                    (*root)->no_entries++;
                }
            }
        }
    }
    else
    {
        printf("Invalid rectangle pushed");
    }
}

struct insert_return_type insert2(struct node *in_node, struct rectangle *rect)
{
    struct insert_return_type result;
    if (in_node->level == 0) // if leaf found
    {
        struct member *temp = (struct member *)malloc(sizeof(struct member));
        if (in_node->no_entries == MAX_MEM)// if full
        {
            temp->rect = *rect;
            temp->child = (struct node *)id++; /* storing id*/
            result.n = 2;
            result.upd_arr = split_Quadratic(in_node, *temp); //  calling split
        }
        else // if not full simply add the member to the node
        {
            (in_node->member[in_node->no_entries]).rect = *rect;
            (in_node->member[in_node->no_entries]).child = (struct node *)id++;
            in_node->no_entries++;
            result.n = 1;
            result.upd_arr = (struct member *)malloc(sizeof(struct member));
            result.upd_arr[0].rect = find_mbr(in_node->member, in_node->no_entries);
            result.upd_arr[0].child = in_node;
        }
    }
    else // if not leaf
    {
        int n = find_member(in_node, rect); // finding appropriate child
        struct insert_return_type temp = insert2(in_node->member[n].child, rect); // recursively inserting in child
        if (temp.n == 1) // if no split in child
        {
            in_node->member[n] = temp.upd_arr[0];
            result.n = 1;
            result.upd_arr = (struct member *)malloc(sizeof(struct member));
            result.upd_arr[0].rect = find_mbr(in_node->member, in_node->no_entries);
            result.upd_arr[0].child = in_node;
        }
        else if (temp.n == 2)// if there was split in child
        {
            if (in_node->no_entries == MAX_MEM) // if node is full
            {
                in_node->member[n] = temp.upd_arr[0];
                result.n = 2;
                result.upd_arr = split_Quadratic(in_node, temp.upd_arr[1]); // calling split function
            }
            else
            {
                in_node->member[n] = temp.upd_arr[0];
                in_node->member[in_node->no_entries] = temp.upd_arr[1];
                in_node->no_entries++;
                result.n = 1;
                result.upd_arr = (struct member *)malloc(sizeof(struct member));
                result.upd_arr[0].rect = find_mbr(in_node->member, in_node->no_entries);
                result.upd_arr[0].child = in_node;
            }
        }
    }
    return result;
}

struct member *split_Quadratic(struct node *node, struct member mem) //main functio to perform splitting
{
    int level = node->level; //since the node will be erased later the value of level is stored beforehand
    Rtree_get_branches(node, &mem); //creates the buffer to store member elements
    split_grouping(&s_list, MIN_MEM);//will decide final two groups to be formed

    struct node *nn = getnode(); //new node created to store the extra group created
    nn->level = level; //level of the node updated
    node->level = level; //level of the node updated
    node->no_entries = 0; 
    struct member *upd_arr = load_node(node, nn, &s_list); //function will prepare the final two nodes to be updated in the rtree
    return upd_arr;
}
struct rectangle find_mbr(struct member *members, int entries) //simple function to find the mbr for an array of rectangles
{
    struct rectangle rect;
    if (entries > 0)
    {
        float min, max;
        for (int i = 0; i < DIMENSIONS; i++)
        {
            min = members[0].rect.edges[i];
            max = members[0].rect.edges[i + DIMENSIONS];
            for (int j = 1; j < entries; j++)
            {
                if (members[j].rect.edges[i] < min)
                {
                    min = members[j].rect.edges[i];
                }
                if (members[j].rect.edges[i + DIMENSIONS] > max)
                {
                    max = members[j].rect.edges[i + DIMENSIONS];
                }
            }
            rect.edges[i] = min;
            rect.edges[i + DIMENSIONS] = max;
        }
    }
    else
    {
        intialise_rect(&rect);
    }
    return rect;
}
float min(float a, float b)
{
    float result;
    if (a > b)
    {
        return b;
    }
    else
        return a;
}
float max(float a, float b)
{
    float result;
    if (a > b)
    {
        return a;
    }
    else
        return b;
}
struct rectangle find_temparory_mbr(struct rectangle rect, struct rectangle rect2) //finds the MBR to given two rectangle
{
    struct rectangle resulted_rect;
    for (int i = 0; i < DIMENSIONS; i++)
    {
        resulted_rect.edges[i] = min(rect.edges[i], rect2.edges[i]);//lower edge the minimum of lower edges the two rectangle
        resulted_rect.edges[i + DIMENSIONS] = max(rect.edges[i + DIMENSIONS], rect2.edges[i + DIMENSIONS]);//higher edge the maximum of higher edges the two rectangle
    }
    return resulted_rect;
}
void Rtree_get_branches(struct node *n, struct member *m)
{
    for (int i = 0; i < MAX_MEM; i++)
    {
        member_buffer[i] = n->member[i];//member_buffer is a global varible where we temporarly store the members from a node while splitting
    }
    member_buffer[MAX_MEM] = *m;//new memeber is loaded on the buffer
    buffercount = MAX_MEM + 1;//buffercount increased
    cover_split = member_buffer[0].rect; //cover split stores the MBR for the buffered rectangles
    for (int i = 1; i < MAX_MEM + 1; i++)
    {
        cover_split = find_temparory_mbr(cover_split, member_buffer[i].rect);
    }
    cover_split_area = find_area(cover_split);

    intialise_node(n);//node n is now cleared
}
void split_grouping(struct crude_split_list *s, int min_mem)
{
    Rtree_intialse_split_list(s, buffercount, min_mem);//will intialise the s_list
    Rtree_pick_seeds(s);//function chooses the seeds 
    float area_in_group0, area_in_group1, diff, max_diff;
    int group, better_group, indx;

    while (((s->count[0] + s->count[1]) < s->total) && (s->count[0] + s->min_mem < s->total) && (s->count[1] + s->min_mem < s->total))
    {      //while enough rectangles are there to be so that we can add atleast min_mem rects to the other seed
        max_diff = -1.0;
        for (int i = 0; i < s->total; i++)//each iteration will decide which rect is best to be grouped and to while grouped
        {   
            if (!s->taken[i])//if s is not taken then 
            {
                area_in_group0 = find_area(find_temparory_mbr(member_buffer[i].rect,
                                                              s->seed_cover[0]));
                area_in_group1 = find_area(find_temparory_mbr(member_buffer[i].rect,
                                                              s->seed_cover[1]));
                diff = area_in_group1 - area_in_group0;
                if (diff > 0)
                {
                    group = 0;
                }
                else
                {
                    group = 1;
                    diff = -diff;
                }
                if (diff > max_diff)//if diff is max diff then new combination found for max_diff
                {
                    max_diff = diff;
                    indx = i;
                    better_group = group;
                }
                else if (diff == max_diff &&
                         s->count[group] < s->count[better_group])
                {
                    indx = i;
                    better_group = group;
                }
            }
        }
        Rtree_add_to_group(indx, better_group, s);//simply updates the s_list to notify the meber at indx is to be better_group
    }
    if (s->count[0] + s->count[1] < s->total)//if condition arises that the we have to add all rectangles to one group to ensure that it has atleast min_mem(ie M/2) members
    {
        if (s->count[0] + s->min_mem >= s->total)
        {
            group = 1;
        }
        else
        {
            group = 0;
        }
        for (int i = 0; i < s->total; i++)
        {
            if (!s->taken[i])
            {
                Rtree_add_to_group(i, group, s);
            }
        }
    }
    assert(s->count[0] + s->count[1] == s->total);//used for debugging
    assert(s->count[0] >= s->min_mem && s->count[1] >= s->min_mem);//used for debugging
}
void Rtree_intialse_split_list(struct crude_split_list *s, int total, int min_mem)
{   //used for providing default values to s_list
    s->count[0] = 0;
    s->count[1] = 0;
    s->min_mem = min_mem;
    s->total = total;
    intialise_rect(&(s->seed_cover[0]));
    intialise_rect(&(s->seed_cover[1]));
    s->seed_area[0] = 0;
    s->seed_area[1] = 0;
    for (int i = 0; i < total; i++)
    {
        s->taken[i] = 0;
        s->split_list[i] = -1;
    }
}
void Rtree_pick_seeds(struct crude_split_list *s)
{   
    int worst = -cover_split_area - 1;//we know for sure that there would be worse than this 
    int seed0, seed1;
    int wasted;
    int area[s->total];
    for (int i = 0; i < s->total; i++)//we can before hand find area of each rectangle in buffer
    {
        area[i] = find_area(member_buffer[i].rect);
    }
    for (int i = 0; i < (s->total - 1); i++)//follwing loops will select (nC2) two pairs from the buffer_array  
    {
        for (int j = i + 1; j < s->total; j++)
        {
            wasted = find_area(find_temparory_mbr(member_buffer[i].rect, member_buffer[j].rect)) - area[i] - area[j];//calculates the empty space in the MBR
            if (wasted > worst)//if wasted is greater than worst then we have found the new potential seed pairs
            {
                worst = wasted;
                seed0 = i;
                seed1 = j;
            }
        }
    }
    Rtree_add_to_group(seed0, 0, s);//finally seeds are updated in s_list
    Rtree_add_to_group(seed1, 1, s);//finally seeds are updated in s_list
}
void Rtree_add_to_group(int i, int group, struct crude_split_list *s)
{
    s->split_list[i] = group;//i will be inserted in group {group}
    s->taken[i] = 1;
    if (s->count[group] == 0)//if is the seed 
    {
        s->count[group] = 1;
        s->seed_cover[group] = member_buffer[i].rect;
        s->seed_area[group] = find_area(member_buffer[i].rect);
    }
    else//if not the first element in the group
    {
        s->count[group] += 1;
        s->seed_cover[group] = find_temparory_mbr(s->seed_cover[group], member_buffer[i].rect);
        s->seed_area[group] = find_area(s->seed_cover[group]);
    }
}
struct member *load_node(struct node *n1, struct node *n2, struct crude_split_list *s)
{ //this the finally function in splitting process
    /* assmbles all the information and the erases the intiall node
        then creates two new nodes and then puts them on the member array of size 2
        which can then be simply updated in their parent 
     */
    for (int i = 0; i < s->total; i++)
    {
        if (s->split_list[i] == 0)
        {
            add_member(&member_buffer[i], n1);
        }
        else if (s->split_list[i] == 1)
        {
            add_member(&member_buffer[i], n2);
        }
    }
    struct member *upd_arry = (struct member *)malloc(2 * sizeof(struct member));
    upd_arry[0].rect = find_mbr(n1->member, n1->no_entries);
    upd_arry[0].child = n1;
    upd_arry[1].rect = find_mbr(n2->member, n2->no_entries);
    upd_arry[1].child = n2;
    return upd_arry;
}
void add_member(struct member *mem, struct node *n)
{   
    n->member[n->no_entries] = *mem;//adds the member to end of member array of a node
    n->no_entries++; //no_entries is updated
}

/*SEARCH*/
int RTreeSearch(struct node *N, struct rectangle *R, SearchHitCallback shcb)
{
    //copying given data in local variables
    struct node *n = N;
    struct rectangle *r = R;
    int hitCount = 0;
    int i;

    assert(n); // checking if given condition is true or not if not terminate the code
    assert(n->level >= 0);// checking if given condition is true or not if not terminate the code
    assert(r);// checking if given condition is true or not if not terminate the code

    if (n->level > 0) // if internal node
    {
        for (i = 0; i < MAX_MEM; i++)// for all members of node
            if (n->member[i].child && RTreeOverlap(r, &n->member[i].rect)) // if rectangle exist and it overlapped with given rectangle
            {
                hitCount += RTreeSearch(n->member[i].child, R, shcb);// recursively going to leaf
            }
    }
    else // when in leaf
    {
        for (i = 0; i < MAX_MEM; i++)// for all members
            if (n->member[i].child && RTreeOverlap(r, &n->member[i].rect))// if rectangle exist and it overlapped with given rectangle
            {
                hitCount++; // increasing hitcount
                if (shcb)// printing rectangle id
                    if (!shcb((int)n->member[i].child)) // if leaf
                        return hitCount;
            }
    }
    return hitCount;
}
void RTreeRandomRect(struct rectangle *R)
{
    struct rectangle *r = R;
    int i;
    float width;
    for (i = 0; i < DIMENSIONS; i++)
    {
        width = drand48() * (1000 / 4) + 1; // width of rectangle
        r->edges[i] = drand48() * (1000 - width); // for minimum dimensions
        r->edges[i + DIMENSIONS] = r->edges[i] + width; // for maximum dimensions
    }
}
void RTreePrintRect(struct rectangle *R, int depth)
{
    struct rectangle *r = R;
    int i;
    assert(r);// checking if given condition is true or not if not terminate the code
    RTreeTabIn(depth); // to make it look good we have given proper spacing
    printf("rect:\n");
    for (i = 0; i < DIMENSIONS; i++) // printing the coordinates of rectangle
    {
        RTreeTabIn(depth + 1);
        printf("%f\t%f\n", r->edges[i], r->edges[i + DIMENSIONS]);
    }
}
int RTreeOverlap(struct rectangle *R, struct rectangle *S)
{
    struct rectangle *r = R, *s = S;
    int i, j;
    assert(r && s);// checking if given condition is true or not if not terminate the code

    for (i = 0; i < DIMENSIONS; i++)
    {
        j = i + DIMENSIONS; /* index for high sides */
        if (r->edges[i] > s->edges[j] || s->edges[i] > r->edges[j]) // checking if rectangles overlap or not
        {
            return 0; // return 0 if not overlap
        }
    }
    return 1;
}
void RTreeTabIn(int depth)
{
    int i;
    for (i = 0; i < depth; i++)
        putchar('\t');// inserting space
}
