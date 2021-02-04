#!/bin/bash

#put my data in a file
./cbf_test_7dof_pos_ctrl > my_data.dat

#launch gnuplot
gnuplot <<EOF
#gnuplot commands here
set terminal wxt 0 persist
plot "my_data.dat" u 1:2 w lp title "TGT", \
     "my_data.dat" u 1:3 w lp title "CUR", \
     "my_data.dat" u 1:4 w l notitle, \
     "my_data.dat" u 1:5 w l notitle, \
     "my_data.dat" u 1:6 w l notitle, \
     "my_data.dat" u 1:7 w l notitle, \
     "my_data.dat" u 1:8 w l notitle, \
     "my_data.dat" u 1:9 w l notitle, \
     "my_data.dat" u 1:10 w l notitle;
#enable menu buttons
pause mouse close;
EOF
