wget -q --show-progress https://github.com/ZLangJIT/Music.prx/releases/download/music/music.prx -O prx/music_tmp1.prx || exit 1
wget -q --show-progress https://github.com/ZLangJIT/Music.prx/releases/download/music/seplugins.zip || exit 1
cd prx
unzip ../seplugins.zip
ls -l prx
