#!/bin/bash

SCRIPT=$1
URL="http://localhost:8080"

if [ -z "$SCRIPT" ]; then
  echo "Usage: ./run_benchmark_k6.sh get.js"
  exit 1
fi

echo "vus,tps,avg,p50,p90,p95,p99,core1_cpu_max,disk_util_max" > results.csv

for VUS in 1 5 10 20 30 50 75 100 150 200
do
    echo "Running test with $VUS VUs.."

    mpstat -P 1 1 > core1.log &
    MPSTAT_PID=$!

    iostat -dx 1 > disk.log &
    DISK_PID=$!

    OUT=$(k6 run --summary-export=summary.json --env VUS=$VUS $SCRIPT 2>&1)

    kill $MPSTAT_PID
    kill $DISK_PID

    CORE1_IDLE_MIN=$(awk '/^[0-9]/ {if(min=="" || $12 < min) min=$12} END {print min+0}' core1.log)
    CORE1_CPU_MAX=$(echo "100 - $CORE1_IDLE_MIN" | bc)

    DISK_UTIL_MAX=$(awk '/^[a-zA-Z]/ {if(max=="" || $(NF) > max) max=$(NF)} END {print max+0}' disk.log)

    TPS=$(jq '.metrics.http_reqs.rate' summary.json)
    AVG=$(jq '.metrics.http_req_duration.avg' summary.json)
    P50=$(jq '.metrics.http_req_duration["p(50)"]' summary.json)
    P90=$(jq '.metrics.http_req_duration["p(90)"]' summary.json)
    P95=$(jq '.metrics.http_req_duration["p(95)"]' summary.json)
    P99=$(jq '.metrics.http_req_duration["p(99)"]' summary.json)

    echo "$VUS,$TPS,$AVG,$P50,$P90,$P95,$P99,$CORE1_CPU_MAX,$DISK_UTIL_MAX" >> results.csv
done
