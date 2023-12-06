#!/bin/bash

#nombres de threads totaux
thread_counts=(2 4 8 16 32 64)

run_and_measure() {
    program=$1
    thread_count=$2

    start_time=$(date +%s%N)

    if [ "$program" == "philosophers" ] || [ "$program" == "philosophers_tatas" ]; then
        ./$program $thread_count > /dev/null 2>&1
    else
        half_thread_count=$(($thread_count / 2))
        ./$program $half_thread_count $half_thread_count > /dev/null 2>&1
    fi

    end_time=$(date +%s%N)

    duration=$((($end_time - $start_time) / 1000000)) # millisecondes
    echo "$thread_count, $duration" >> "./data/${program}_performance.csv"
}

for program in "writer_reader" "writer_reader_tatas" "producer_consumer" "producer_consumer_tatas" "philosophers" "philosophers_tatas"; do
    echo "threads, duration_ms" > "./data/${program}_performance.csv"
    for count in "${thread_counts[@]}"; do
        for i in {1..5}; do
            run_and_measure $program $count
        done
    done
    #cat "./data/${program}_performance.csv"
done

bash ./test_and_set.sh

python3 "plots.py"
