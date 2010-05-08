#!/bin/bash
GENPATH=$1
: ${GENPATH:="${HOME}/projects"}
lua $GENPATH/genmakefile/src/genmakefile.lua <etc/make/Makefile.luaamf.template >Makefile
