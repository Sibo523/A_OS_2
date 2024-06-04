
# Test case 1: Valid input for the program should run on all lines that doesn't exit
echo "3 3
0 10 21
4 0 17
19 3 0
1" | server

# Test case 2: Graph with too many edges
echo "3 3 
0 10 20 30 20 30 50 2 3 4 
0" | server

# Test case 3: Graph with negative weights
echo "3 3
0 10 -5
15 0 20
7 9 0
0" | server

# Test case 4: Invalid start vertex
echo "3 3
0 10 20
15 0 20
7 9 0
-1" | server

# Test case 5: Zero vertices (exit)
echo "-2 -3" | server

# Test case 6: non int source (exit)
echo "3 3
0 10 20
15 0 20
7 9 0
A" | server