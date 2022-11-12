# R-Tree
### Team members
Chavda Jayrajsinh - 2021CSB1078  
Nikhil Garg  - 2021CSB1114  
Priyanshu Kumar - 2021CSB11125   
### Teaching Assistant
Avadhesh
### Rtree Implementation as a part of CS201(DSA) project.
Implementation of R-Tree and some of its functions such as insert, search, split and find near elements. We have also analysed the algorithms and studied their average time complexities. 
### Average Time Complexity of Rtree Operations :
1. Search : O(logn)
2. Insert : O(M^2logn)
3. Split  : O(M^2)
### How to run the program  
The program can be compiled and run directly in the terminal using gcc.  

### Expected inputs and outputs
Program generates the internally 100 random rectangle and searches a rectangle  and outputs 
the rectangle with that id and prints all rectangles(objects) that are near to it (overlaps the search rectangle)
there is a user interface that allows user to specifiy the dimensions of object to be inserted
user can also perform search where user has to enter the coordinates of his location and enter the distance for which he want to perform near search query
