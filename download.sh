rm -rf prx
mkdir prx
wget -q --show-progress https://github.com/ZLangJIT/Music.prx/releases/download/music/music.prx -O prx/music_tmp.prx || exit 1
wget -q --show-progress https://github.com/ZLangJIT/Music.prx/releases/download/music/seplugins.zip -O prx/seplugins.zip || exit 1
cd prx
unzip seplugins.zip
cd ..
find -name *\.prx -exec ls -l {} \;
