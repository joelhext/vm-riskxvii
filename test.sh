#!/bin/bash

rm *.gcno *.gcda *.gcov
gcc -fprofile-arcs -ftest-coverage -o vm_riskxvii vm_riskxvii.c helper.c operations.c memory_handling.c

output_dir="out"
input_dir="in"

for input_file in "${input_dir}"/*.in; do
    echo "Running testcase: ${input_file}"
    filename="$(basename "${input_file}" .in)"
    mi_file="testcases/${filename}.mi"

    # Run vm_riskxvii with the .mi file as the first argument, and feed the .in file to stdin. 
    # Save the output to the output directory with a .out extension
    ./vm_riskxvii "${mi_file}" < "${input_file}" > "${output_dir}/${filename}.out"
done

# Coverage logs (gcov) are generated in the same directory as the source files
gcov vm_riskxvii-vm_riskxvii vm_riskxvii-helper vm_riskxvii-operations vm_riskxvii-memory_handling
