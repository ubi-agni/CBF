#!/bin/bash

# This script is useful only to members of the AG neuroinformatics at Bielefeld University

./waf configure --with-xsd=/homes/fschmidt/local/opt/xsd-3.2.0-i686-linux-gnu/bin/xsd --with-xsd-include-dir=$HOME/local/opt/xsd-3.2.0-i686-linux-gnu/libxsd/ --with-kdl-dir=/vol/tim --with-eigen2-include-dir=/vol/numerik/ --with-kdl-chain-viz-dir=/vol/tim --prefix=/vol/nirobots/ $*
