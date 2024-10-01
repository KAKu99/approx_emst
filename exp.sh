#!/bin/bash

# This script is used to run the experiments for the paper.
timestamp=$(date +"%Y-%m-%d_%H%M%S")
output_file="output_${timestamp}.txt"

# make file if not exists
touch $output_file

# get absolute path of the output file
output_file=$(realpath $output_file)

# experiment settings
# data_folder_path is command line argument 1
data_folder_path=$1
dataset=0
percent=100
k=5
threads=0


echo "Running experiment with dataset $dataset and percent $percent." >> $output_file
echo "Experiment settings" >> $output_file
echo "data_folder_path: $data_folder_path" >> $output_file
echo "dataset: $dataset" >> $output_file
echo "percent: $percent" >> $output_file
echo "k: $k" >> $output_file
echo "#threads(0 means max size): $threads" >> $output_file

echo "Running experiment with dataset $dataset and percent $percent."
echo "Experiment settings"
echo "data_folder_path: $data_folder_path"
echo "dataset: $dataset"
echo "percent: $percent"
echo "k: $k"
echo "#threads(0 means max size): $threads"

# timeout 2h ./main.out $dataset >> $output_file
./main.out $data_folder_path $dataset $percent $k $threads >> $output_file

# gey exit status of the command
exit_status=$?

# judge by status
if [ $exit_status -eq 124 ]; then
    echo "Program timed out"
elif [ $exit_status -eq 0 ]; then
    echo "Program finished successfully"
else
    echo "Error: status code=$exit_status"
fi