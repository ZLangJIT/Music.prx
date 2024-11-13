function compile() {
    echo "CXX $2.cpp"
    psp-g++ -I $PSPSDK/include -I $DIR/include -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic -fno-exceptions -fno-rtti -c -o $2.o $2.cpp || exit 1
}

function link() {
    echo "link ELF $2"
    psp-g++ -D_PSP_FW_VERSION=371 -L$DIR/lib -L$PSPSDK/lib -Wl,-q,-T$PSPSDK/lib/linkfile.prx -nostartfiles *.o -lpspkernel -o $1.elf || exit 1
    
    echo "generating PRX $2"
    psp-fixup-imports $1.elf || exit 1
    psp-prxgen $1.elf $DIR/$1.prx || exit 1
}

function build_plugin() {
    cd $DIR/plugins/$1 || exit 1
    echo "PSP_MODULE_INFO(Music2_Plugin__$1, 0x1000, 1, 1);" > $DIR/music2_plugin_generated.h
    ./make.sh || exit 1
    link $1 || exit 1
    cd $DIR
    mv -v $1.prx seplugins/music2/plugins/$1.prx
}

