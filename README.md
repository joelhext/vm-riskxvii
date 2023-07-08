# VM RISKXVII - RISC-V Simulator

This repository contains a simplified RISC-V simulator, VM RISKXVII, written in C. This was developed as part of a coursework assignment. The objective of the assignment was to understand and implement the RISC-V architecture in a low-level programming language (C), and to write and test programs that run on the implemented virtual machine.

Final mark:
>34.82 / 35

Notable feedback included:
* Switch statement in main function could be moved to another file to increase code clarity.
* No handling for the edge case where a memory store command targets the address of a read operation within the virtual routines.

An extra requirement for the assignment was that the program's binary should not exceed 20kb. This is why I have used the `-Os` flag when compiling.

## About

VM RISKXVII is a basic virtual machine that simulates a subset of the RISC-V architecture. It has been designed to execute RISC-V binary files and produces an output depending upon the instructions within the binary.

### Instructions

The simulator supports a select set of RISC-V instructions such as `add`, `sub`, `mul`, `div`, as well as various shifting and bitwise operations. Additionally, the simulator includes rudimentary memory handling capabilities, allowing for basic interactions with memory.

Each instruction in RISKVII follows a similar format to RISC-V, with opcodes and operands specified in a 32-bit instruction. The opcode is specified in the lower 7 bits, with the remaining bits used to specify the operands.

### Opcodes

RISKVII uses a set of 25 opcodes that include a mix of arithmetic operations, bitwise operations, load and store operations, and control flow operations. Some opcodes correspond to those found in the RISC-V specification, while others are specific to the RISKVII VM.

### Memory Layout

The RISKVII VM is implemented with a total of 4096 bytes (4KB) of memory. This includes:

- A 1KB instruction memory area (at addresses 0x0000-0x03FF), where instructions to be executed by the VM are stored.
- A 1KB data memory area (at addresses 0x0400-0x07FF), where data can be read from or written to by the VM.
- A 2KB area for memory-mapped I/O (at addresses 0x0800-0x0FFF). 

## Building

You can build the VM RISKXVII by using the provided Makefile:

```
make
```

## Testing

A set of test cases is provided in the `testcases/` directory. Each test case is a RISC-V binary file that can be fed into the VM RISKXVII. The Makefile includes a script for running all test cases and summarizing the coverage of each component.

To run the tests and generate a coverage report:

```
make test
```

The coverage is reported as follows:

`Lines executed:61.72% of 789`


### Example Test Cases

This repository includes the source code for three of the test cases that can be found in the `testcases/examples/` directory. For all testcases, the input and output files can be found in `in/` and `out/` directories respectively.


## Docker Support

This repository includes a `Dockerfile` that creates a Ubuntu instance, since my host machine uses the arm64 architecture but the amd64 architecture was necessary for consistency in testcases. During the build process, I mounted the folder in my host machine to the container using the `-v` flag.

You can build and run the project using Docker with the following commands:

```bash
docker build -t vm_riskxvii .
docker run -it -v /path/to/vm_riskxvii:/vm_riskxvii vm_riskxvii
```

*WARNING:* the original Dockerfile installs the RISC-V GNU Compiler Toolchain, which was used to compile the binary files in the `testcases/` directory. It requires multiple GB of storage space, so it has been commented out.

## Contributing

While this project was developed for a coursework assignment and is not actively maintained, contributions are always welcome. Feel free to open an issue or submit a pull request if you have any enhancements or bug fixes.

## License

This project is licensed under the MIT License. See `LICENSE` for more details.
