# Source: http://4ch.mooo.com/4/watcom/wat.sh
# with modifications to simulate this environment on Jon's Linux system
#
# Note that nothing in this project appears to use ROOT or REXX variables
# ------BEGIN-------
#echo Open Watcom Build Environment
ROOT=/home/sparky4/p/osfree
watcomver=""

if [ -d /usr/src/open-watcom-v2/rel ]; then
    # Jon's Linux system
    watcom=/usr/src/open-watcom-v2/rel
    WATCOM=$watcom
else
    # original sparky4 config
    watcom=/dos/fdos/watcom${watcomver}
    WATCOM=$watcom
fi

OS=LINUX
ENV=LINUX
SHELL=Bourne
REXX=/usr/bin/rexx

# use 64-bit Linux binaries on 64-bit.
# NTS: watcomver is never set, therefore this currently has NO EFFECT.
if [ "${watcomver}" == 2 ]; then
	MACHINE_TYPE=`uname -m`
	if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	        PEE="64"
	else
	        PEE=""
	fi
else
	PEE=""
fi

TOOLS=$ROOT/bin/tools
INCLUDE=$watcom/h:$watcom/h/dos:$watcom/lh:$watcom/h/os2
WIPFC=$watcom/wipfc
LIB=$watcom/lib286/dos:$watcom/lib386:$watcom/lib386/dos:$watcom/lib286:$ROOT/lib
export OS REXX WATCOM ROOT TOOLS INCLUDE LIB WIPFC

if [ -d /usr/src/open-watcom-v2/rel ]; then
    # Jon's Linux system
    cur=`pwd`
    cd $WATCOM/.. || exit 1
    . ./setvars.sh
    cd "$cur" || exit 1
else
    # original sparky4 config
    . ~/setvars.sh
fi

PATH=$watcom/binl${PEE}:$TOOLS:$ROOT:$PATH
WD_PATH=$watcom/binl${PEE}

TERM=xterm-16color
export OS ENV REXX WATCOM ROOT TOOLS INCLUDE LIB PATH WD_PATH WIPFC TERM

alias wcp="wcl -lr -l=dos -0 "
if [ ! -d "$watcom" ]; then
	echo "INSTALL OPEN WATCOM AND EDIT THIS FILE TO THE CORRESPONDING DIRECTORY!!!!"
fi

#if [ ! -d "$watcom/binl64" ]; then
#	ln -s $watcom/binl64 $watcom/binl
#fi

#if [  -e "$HOME/setvars.sh" ]; then
#	length=$ROOT
#	((length--))
#	[ "${filename:$length:1}" == "2" ] && . ./setvars.sh
#	else if [ ! -e "setvars.sh" ]; then
#	wget http://4ch.mooo.com/4/watcom/setvars.sh
#	chmod +x ~/setvars.sh
#fi
#fi
#. ~/aliasyonet.sh
