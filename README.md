# 3TST - A Steiner Tree Heuristic

3-Terminal Steiner Tree (3TST) is a heuristic for the Steiner tree problem. Intuitively, the heuristic works
by replacing 3-terminal subtrees of a prospective solution by lighter 3-terminal subtrees. This process
is repeated until no significant improvement is observed in a reasonable amount of time. 

## Compiling: 

Open a windows/linux terminal and execute the following command 
to clone this repository. 

```sh
git clone https://github.com/AutoProving/3TST.git
```
Enter the main folder `3TST` and type `make` to compile the program. 

An executable called `3TST.exe` will be created. 

## Usage

The simplest way of executing the program is to run the following command.
It will read the input graph from the standard input and print a Steiner tree
on the standard output. This is a deterministic procedure. It can be stopped
manually using a `SIGINT` or a `SIGTERM`.

```sh
./3TST.exe < examples/instance1.gr
```

The `timeout` command can be used to run it with a time limit.

```sh
timeout -s TERM 10s ./3TST.exe < examples/instance1.gr
```

The option `-h` or `--help` prints the help message

```sh
./3TST --help
```

The option `-r` or `--random` enables the use of a randomised procedure. This option makes
the program run until it receives a SIGTERM or SIGINT otherwise it will never stop.
In this case the `timeout` command is useful.

```sh
timeout -s TERM 90s ./3TST --random < examples/instance1.gr
```

It is possible to give a specific seed to initialise the random number generator using
the flag `-s` or `--seed`. This flag have effect only with the `--random` option.
In the example below, the seed is 10.

```sh
./3TST.exe -r -s 10 < examples/instance1.gr
```

With the option `-i` or `--improve` we can execute the program on a graph instance
together with an initial Steiner tree. In this case, the program will try to improve
the Steiner tree. This is a deterministic procedure. `--random` and
`--seed` have no effect with `--improve`.

```sh
cat examples/instance1.gr examples/steinertree1.ost | ./3TST.exe -i
```


## Input/Output Formats

The input format for graphs and the output format for Steiner trees are the
same used at PACE Challenge 2018. Please refer to the file [INPUT_OUTPUT.md](INPUT_OUTPUT.md)
for a description of these formats. Alternatively, please read Sections
A and B of the following link:

[https://pacechallenge.org/2018/steiner-tree/](https://pacechallenge.org/2018/steiner-tree/)

The `examples` folder contains some examples from the PACE Challenge.

## Bug Reports and User Feedback

Please report bugs or ask questions using the issue tracker a [https://github.com/AutoProving/3TST/issues](https://github.com/AutoProving/3TST/issues)

## Citation: 

To cite our heuristics, please refer to the following paper.

Emmanuel Arrighi, Mateus de Oliveira Oliveira. Three is Enough for Steiner Trees. 
19th Symposium on Experimental Algorithms.

## Acknowledgements: 

Emmanuel Arrighi acknowledges support from the Research Council of Norway in the
context of the project Parameterized Complexity for Practical Computing (Grant no. 274526)

Mateus de Oliveira Oliveira acknowledges support from the Research Council of 
Norway in the context of the project Automated Theorem Proving from the Mindset of 
Parameterized Complexity Theory (Grant no. 288761). 

We also acknowledge support from the Sigma2 Network (Proj. no. NN9535K)
