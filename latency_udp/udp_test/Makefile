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


#Run udp jitter test
OUTPUT_FILE = hostout.txt
PIN_CPU = 2
RUN_RT = 1
SLEEP_DELAY = 60
SAMPLES = 1000
TOTAL_SAMPLES=10000
IP_SRC = 60.60.0.1
IP_LOCAL = 60.60.0.2
IP_DST = 60.60.0.2
IP_NET = 60.60.0.0
PORT = 5555
IF_NAME = uesimtun0
USE_DAG =1
DAG_OUTPUT = capture/dag.dat
DAG_CAPTURE = capture/source
SKIP=10000
RECORDS=$$(( $(SAMPLES) - $(SKIP) )) 
RUNS=$$(($(TOTAL_SAMPLES) / $(SAMPLES)))


all:
	#rm $(DAG_OUTPUT)*
	for i in `seq 1 10`; do\
		${MAKE} start_dag;\
		${MAKE} test_remote I=$$i;\
		${MAKE} stop_dag;\
		${MAKE} analyse_dag I=$$i;\
	done
	./parse.sh $(DAG_OUTPUT) $(OUTPUT_FILE)


test_remote:
	@echo " #################################################"
	@echo " Starting udp rtt test	                 	 "
	@echo " #################################################"
	sleep $(SLEEP_DELAY)
	sysctl -w net.ipv4.ping_group_range="0 0"
	ifconfig $(IF_NAME) $(IP_LOCAL) up
	$(PIN_PATH)/pininit
	$(PIN_PATH)/pin $(PIN_CPU) $(RUN_RT) "./source/udp_test -n $(SAMPLES) -c $(PIN_CPU) -f $(I)_$(OUTPUT_FILE) -i $(IP_DST) -p $(PORT)"
	$(PIN_PATH)/pinreset
	@echo " #################################################"
	@echo " Finished application+OS ping test                "
	@echo " #################################################"

test_emu:
	@echo " #################################################"
	@echo " Starting udp rtt test                           "
	@echo " #################################################"
	ifconfig nf0 $(IP_SRC) up
	route add -net $(IP_NET)  netmask 255.255.255.0 dev nf0
	arp -s $(IP_DST)  00:0f:53:0d:d1:51 -i nf0
	sleep $(SLEEP_DELAY)
	sysctl -w net.ipv4.ping_group_range="0 0"
	$(PIN_PATH)/pininit
	$(PIN_PATH)/pin $(PIN_CPU) $(RUN_RT) "./source/udp_test -n $(SAMPLES) -c $(PIN_CPU) -f $(OUTPUT_FILE) -i $(IP_DST) -p $(PORT)"
	$(PIN_PATH)/pinreset
	@echo " #################################################"
	@echo " Finished application+OS ping test                "
	@echo " #################################################"


server:
	@echo " #################################################"
	@echo " Starting udp test  in server mode               "
	@echo " #################################################"
	sleep $(SLEEP_DELAY)
	ifconfig $(IF_NAME) $(IP_LOCAL) up
	ethtool -C $(IF_NAME) rx-usecs 0
	$(PIN_PATH)/pininit
	$(PIN_PATH)/pin $(PIN_CPU) $(RUN_RT) "./source/udp_test -s  -c $(PIN_CPU) -p $(PORT)"
	$(PIN_PATH)/pinreset
	@echo " #################################################"
	@echo " Finished application+OS ping test                "
	@echo " #################################################"

start_dag:
	@echo "Starting DAG Capture..."
ifeq ($(USE_DAG),1)
	dagload ;\
	$(CAMIO_PATH)/dag_capture/bin/dag_capture & echo $$! > capture.PID
endif

stop_dag:
	@echo "Stopping DAG Capture..."
ifeq ($(USE_DAG), 1)
	sleep 1
	kill `cat capture.PID`
	rm capture.PID
	sleep 5
endif

analyse_dag:
ifeq ($(USE_DAG),1)
	cp /tmp/dag_cap_A $(DAG_CAPTURE)/dag_cap_A$(I);
	cp /tmp/dag_cap_B $(DAG_CAPTURE)/dag_cap_B$(I);
	$(CAMIO_PATH)/dag_analyse_join/bin/dag_analyse -i /tmp/dag_cap_B -I /tmp/dag_cap_A -o $(DAG_OUTPUT)$(I) -f $(SKIP) -F $(SKIP) -l $(RECORDS) -p 2 -c
endif
clean:
	rm -f *$(OUTPUT_FILE) 
	rm -f source/*.o source/udp_test
build:
	gcc source/udp_test.c -o source/udp_test
