#!/bin/bash

source /root/test_state.txt
if [ $RUN_STATE -eq 1 ]
then
  source /root/latency/run_test.sh
fi

