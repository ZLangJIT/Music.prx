rm -rf prx
mkdir prx
cd prx
wget -q --show-progress https://github.com/krystalgamer/psptoolchain/releases/latest/download/toolchain.tar.xz
echo "extracting..."
tar -xf toolchain.tar.xz
wget -q --show-progress https://github.com/ZLangJIT/Music.prx/releases/download/music/music.prx -O music_tmp.prx || exit 1
wget -q --show-progress https://github.com/ZLangJIT/Music.prx/releases/download/music/seplugins.zip -O seplugins.zip || exit 1
unzip seplugins.zip
cd ..
find -name *\.prx -exec bash -c "ls -l {} ; ls -lh {}" \;
