#!/bin/bash
# Copyright (c) 2016, Noa Zilberman
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of the project, the copyright holder nor the names of its
#  contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#!/bin/bash
SUCCESS_MSG="Test completed successfully"
START_MSG="Started running a latency test"
export TEST_PATH=/ueransim/latency/
TEST_NUMBER=3
IS_SERVER=0
IS_BYPASS=0
START_SERVER=0
export PIN_PATH=/ueransim/latency/tools/pin/
export CAMIO_PATH=/ueransim/latency/tools/qjump-camio-tools

cd $TEST_PATH
echo "Sending start email"



#udp test
if [ $TEST_NUMBER -eq 3 ]
then
	if [ $IS_SERVER -eq 0 ]
	then
		echo "Running udp test"
		make -C udp_test
	else
		echo "Running udp server test, press ctrl+break to end"
		make -C udp_test server
	fi
fi



#EMU ICMP test
if [ $TEST_NUMBER -eq 10 ]
then
	echo "Running Emu ICMP test..."
	make -C icmp_test
fi



echo "RUN_STATE=0" > /root/test_state.txt