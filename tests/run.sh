#!/bin/sh

# Limits
time_limit="300"
mem_limit="5000000"
n_threads=4

ulimit -t unlimited
ulimit -a
shopt -s nullglob

fileloc="simple/*.cnf"
solver="../satomi"
output="results"

mkdir -p $output

# Remove everything that has been done
for file in $output/*
do
	echo "Deleting file: $file"
	rm "$file"
	status=$?
	if [ $status -ne 0 ]; then
		echo "[Error] Can't delete file: $file."
		exit
	fi
done

# Create TODO list
rm -f todo
for file in $fileloc
do
	filename=$(basename "$file")
	todo="/usr/bin/time -lp $solver $file > $output/$filename.out 2>&1"
	echo "$todo" >> todo
done

n_lines=$(wc -l todo |  awk '{print $1}')
echo "Done creating TODO with $n_lines of problems"

# Create random order
echo "Randomizing order of execution of files"
perl -MList::Util -e 'print List::Util::shuffle <>' todo > todo_rnd

# Create per-core todos
rm -f todo_rnd_*
let n_pc=n_lines/n_threads
remain=$((n_lines - n_pc * n_threads))
start=0
echo "Creating per-core TODOs"
for ((i = 0; i < n_threads ; i++))
do
	echo todo_rnd_$i.sh
	echo "ulimit -t $time_limit" > todo_rnd_$i.sh
	echo "ulimit -v $mem_limit" >> todo_rnd_$i.sh
	echo "ulimit -a" >> todo_rnd_$i.sh
	typeset -i i
	typeset -i n_pc
	typeset -i start
	start=$((start + n_pc))
	head -n $start todo_rnd | tail -n $n_pc >> todo_rnd_$i.sh
	chmod +x todo_rnd_$i.sh
done

let i--
tail -n $remain todo_rnd >> todo_rnd_$i.sh

# Execute todos
rm -f out_*
echo "Executing TODO's..."
for ((i = 0; i < n_threads ; i++))
do
	nohup ./todo_rnd_$i.sh > out_$i &
	echo "OK"
done
