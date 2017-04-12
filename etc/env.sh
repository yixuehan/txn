export PROROOT=~/firstPro
export BOOST_HOME=/home/developer/boost_1_63_0
export BOOST_INCLUDEDIR=${BOOST_HOME}/include
export BOOST_LIBRARYDIR=${BOOST_HOME}/lib/release
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${BOOST_HOME}/lib/release:${PROROOT}/lib

alias cds='cd $PROROOT/src'
alias cdi='cd $PROROOT/include'
alias cdm='cd $PROROOT/mak'
alias cde='cd $PROROOT/etc'
alias cdl='cd $PROROOT/log'
alias cdb='cd $PROROOT/bin'
alias cdp='cd $PROROOT'
alias cdlib='cd $PROROOT/lib'
alias cpmake='cp $PROROOT/mak/make.sh .'
alias rm='rm -i'

ulimit -c unlimited
