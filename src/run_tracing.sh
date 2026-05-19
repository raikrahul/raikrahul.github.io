#!/bin/bash
# Tracing orchestration script for stack crash investigation
# Set up paths
EXP_DIR="/home/r/Desktop/study/interview_systems/stanford140_fs/experimentation"
BUGGY_EXE="/home/r/Desktop/study/interview_systems/stanford140_fs/assign0/buggy"

echo "=== Loading kprobe_crash kernel module ==="
echo 1 | sudo -S insmod $EXP_DIR/kprobe_crash.ko

echo "=== Starting bpftrace in the background ==="
echo 1 | sudo -S bpftrace $EXP_DIR/trace_crash.bt > $EXP_DIR/ebpf_crash.log &
BPF_PID=$!
sleep 2

echo "=== Running buggy to trigger crash (segfault) ==="
# It will crash, so ignore the shell return error
$BUGGY_EXE "abcdefgh" 4

sleep 2
echo "=== Terminating bpftrace ==="
echo 1 | sudo -S kill $BPF_PID

echo "=== Unloading kprobe_crash kernel module ==="
echo 1 | sudo -S rmmod kprobe_crash

echo "=== Collecting dmesg logs ==="
dmesg | tail -30 > $EXP_DIR/dmesg_crash.log

echo "=== Tracing completed successfully! ==="
