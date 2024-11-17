if [ $# == 0 ] ; then
	cmd="-c"
	args="\"export TERM=xterm-256color ; cd /termux_pwd ; exec bash\""
else
	cmd="-c"
	args="\"export TERM=xterm-256color ; cd /termux_pwd ; $@\""
fi

export TERMUX_PREFIX=$(cd ; cd ../.. ; pwd)
eval proot-distro login ubuntu \
	--isolated --bind "$(pwd):/termux_pwd" \
	-- env -i bash -i $cmd $args
