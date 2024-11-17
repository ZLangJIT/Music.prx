if [ ! -e /usr/bin/apt-extracttemplates ] ; then
	echo "installing apt-utils"
	sudo apt install -y apt-utils
fi
export PSPDEV=/opt/local/pspdev
export PATH=$PSPDEV/bin:$PATH
if [ ! -e $PSPDEV ] ; then
	mkdir -p $PSPDEV
fi
if [ ! -e $PSPDEV/bin/psp-config ] ; then
	if [ ! -e ubuntu_pspsdk ] ; then
		mkdir ubuntu_pspsdk
	fi
	cd ubuntu_pspsdk
	if [ ! -e pspdev ] ; then
		echo "cloning pspdev..."
		git clone https://github.com/pspdev/pspdev
	fi
	echo "building pspdev..."
	cd pspdev
	./prepare.sh || exit 1
	./build-all.sh || exit 1
	cd ..
	cd ..
fi
psp-config --sdk-path
exit 0
cd music1
make
cd ..
cd music2
./make.sh
cd ..
rm -rf prx
mkdir prx
ln -s $(pwd)/music1/music.prx prx/music_tmp.prx
ln -s $(pwd)/music2/seplugins prx/seplugins
find -name *\.prx -exec bash -c "ls -l {} ; ls -lh {}" \;
