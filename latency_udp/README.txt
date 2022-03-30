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

This file documents the process required to run the latency tests.
Note that the process may change between platforms and OS.

Test Details: 
=============

The following summarises the tests provided with this package

.
├── app_os                : measure combined latency of user space application and the os 
├── app_test              : measure the latency of TSC in user space (only)
├── client_server_test    : measure the latency between the user space application in client and server, running memaslap.
├── exanic_test           : measure the latency of X10 NIC
├── icmp_test             : measure the latency of ICMP message
├── kernel_coldstart_test : measure the latency of TSC in the kernel early during OS init
├── kernel_test           : measure the latency of TSC in the kernel
├── motivation_test       : measure the effect of latency on different benchmarks performance
├── network_switch_test   : measure the port-to-port latency experienced by packets through different switches.
├── pcie_test             : measure the latency of host interconnect (PCIe)  
├── root files            : provides files that should be places under root folder (refer to root_files/README.txt) 
├── sfc_test              : measure the latency of Solarflare NIC (SFN8522)
├── udp_test              : measure the latency of UDP message
├── vm_test               : start the VM, which runs one of theprevious experiments 


Step 1: Hardware setup
======================

Contents of client system: NIC card, DAG card. NetFPGA SUME card is optional.
Contents of server system: NIC card, NetFPGA SUME card.
In BIOS:
Disable frequency scaling, turbo mode, hyper threading and relevant power save modes.
In OS:
Disable frequency scaling and enable hugepages (needed for PCIe related tests):
1. vim /etc/default/grub
2. GRUB_CMDLINE_LINUX_DEFAULT="intel_pstate=disable hugepagesz=1GB default_hugepagesz=1GB hugepages=8"
3. save and exit
4. update-grub

Step 2: Startup Configuration
=============================

1. Goto /latency_experiments/root_files. This folder contains files that should be copied to the root folder as well as other locations. 
   Follow the instructions in the README.txt file of this folder.
2. To get email notifications: install utilities such as mailutils and postfix.
   Examples are to set these up can be found in many places, e.g. https://easyengine.io/tutorials/linux/ubuntu-postfix-gmail-smtp/


Step 3: Tuning parameters/configuration and running tests:
==========================================================

Folder structure:
-----------------

latency_experiments      : Main folder holding the tests
├── run_test.sh          : Used to configure which tests to run    
├── {your_test}          : To tune the parameters for {your_tests}, check the Makefile or .sh file inside the project  
│   ├── Makefile
│   ├── README.txt
│   ├── {your_test}.sh


1. Enter the /latency_experiments folder
2. Upate run_test.sh parameters EMAIL_ADDR and TEST_PATH with your email and machine test path values
4. Select the tests you'd like to run (by setting the value to 1)
5. Under each test folder there is a Makefile and .sh file. Change the parameters in the file to match your test purposes. The default values are those
used in our paper: TBD.