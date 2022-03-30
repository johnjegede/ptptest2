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

samples=1000
file_in='dag_all.txt'
file_out='dag_results.txt'

file2_in='host_all.txt'
file2_out='host_results.txt'

rm $file_in
cat $1* >> $file_in
cat $file_in | cut -d"," -f 14 |cut -d"," -f 1 | sort -n  > tmp
lines=`wc -l tmp | cut -d" " -f 1`
nawk -vlines=$lines '{if ((i%1000==0)||(i==lines-1)){printf ("%g %s\n",(float)i/lines,$1);} i++;}' tmp > $file_out
rm tmp

rm $file2_in
cat *$2 >> $file2_in
cat $file2_in | grep -v m | grep -v c | grep -v '%' |  sort -n  > tmp
lines=`wc -l tmp | cut -d" " -f 1`
nawk -vlines=$lines '{if ((i%1000==0)||(i==lines-1)){printf ("%g %s\n",(float)i/lines,$1);} i++;}' tmp > $file2_out
#rm tmp



