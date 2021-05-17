# Description of the Input and Output formats for graphs and Steiner Trees

The input and output formats used in our program are the same used by the 
PACE Challenge 2018 (Track 3, Heuristic). The description below was extracted
from the link [https://pacechallenge.org/2018/steiner-tree/](https://pacechallenge.org/2018/steiner-tree/)


## Graph format (.gr)

We use the format ".gr" to specify graphs. 

The file starts with a line ‘SECTION Graph’. The next two lines are of the 
form ‘Nodes #nodes’ and ‘Edges #edges’, always in that order, where #nodes
is the number of vertices and #edges is the number of edges of the graph. The #edges
next lines are of the form ‘E u v w’ where u and v are integers between 1 and #nodes
representing an edge between u and v of weight the positive integer w.
The following line reads ‘END’ and finishes the list of edges.

There is then a section Terminals announced by two lines ‘SECTION Terminals’ and ‘Terminals #terminals’
where #terminals is the number of terminals. The next #terminals lines are of the form ‘T u’ where u is an
integer from 1 to #nodes, which means that u is a terminal. Again, the section ends with the line ‘END’.

The file ends with a subsequent line ‘EOF’. Here is an example of a small graph.

```
SECTION Graph
Nodes 5
Edges 6
E 1 2 1
E 1 4 3
E 3 2 3
E 2 4 4
E 3 5 10
E 4 5 1
END

SECTION Terminals
Terminals 2
T 2
T 4
END

EOF
```

# Steiner Trees (.ost)

We use the format ".ost" to specify Steiner trees. The file starts with the line ‘VALUE x’
where x is the weight of the found Steiner tree. The next lines are of the form ‘u v’ where
u and v are two vertices of the graph linked by an edge and lists all the edges of the
found Steiner tree. Here is a small example.

```
VALUE 20
1 3
3 5
7 3
10 7
7 22
```
