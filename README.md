# 3TST - A Steiner Tree Heuristics

3TST is a heuristics for the Steiner tree problem. 

## Compiling: 

Open a windows/linux terminal and execute the following command 
to clone this repository. 

(TO DO)

Enter the main folder "3TST" and type "make" to compile the program. 

An executable called 3TST.exe will be created. 

## Usage

The simplest way of executing the program is to run the following command. It will read the input file from the standard input. 

```
./3TST.exe < inputFile.gr   
```

As a second option you can use the flag "-s" to specify the seed that will initialize the random number generator. In the example below, the seed is 10. 

```
./3TST.exe -s 10 < inputFile.gr
```

## Input/Output Formats

The input format for graphs and the output format for Steiner trees are the same used at PACE Challenge 2018. Please refer to the file inputOutputFormats.txt for a description of these formats. Alternatively, please read Sections A and B of the following link:

https://pacechallenge.org/2018/steiner-tree/  


## Citation: 

To cite our heuristics, please refer to the following paper.

Emmanuel Arrighi, Mateus de Oliveira Oliveira. Three is Enough for Steiner Trees. 
19th Symposium on Experimental Algorithms.

## Acknowledgements: 

Emmanuel Arrighi acknowledges support from the Research Council of Norway in the context of the project Parameterized Complexity for Practical Computing (Grant no. 274526)

Mateus de Oliveira Oliveira acknowledges support from the Research Council of Norway in the context of the project Automated Theorem Proving from the Mindset of Parameterized Complexity Theory (Grant no. 288761). 

We also acknowledge support from the Sigma2 Network (Proj. no. NN9535K)
