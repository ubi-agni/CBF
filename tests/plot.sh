#!/bin/bash

#put my data in a file
./cbf_test_7dof_pos_ctrl > my_data.dat

#launch gnuplot
gnuplot <<EOF
#gnuplot commands here
set terminal wxt 0 persist
plot "my_data.dat" u 1:2 w lp, \
     "my_data.dat" u 1:3 w lp

EOF


