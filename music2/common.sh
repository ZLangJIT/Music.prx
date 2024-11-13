function compile_c() {
    echo "CC $1.c"
    psp-gcc -I $PSPSDK/include -I $DIR/include -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic -c -o $1.o $1.c || exit 1
    printf "$1.o " >> $DIR/file_list
}

function compile() {
    echo "CXX $1.cpp"
    psp-g++ -I $PSPSDK/include -I $DIR/include -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic -fno-exceptions -fno-rtti -c -o $1.o $1.cpp || exit 1
    printf "$1.o " >> $DIR/file_list
}

function link() {
    echo "build exports for $1"
    psp-build-exports -b $DIR/exports.exp > exports.c
    compile_c exports
    
    echo "link ELF $1"
    psp-gcc -D_PSP_FW_VERSION=371 -L$DIR/lib -L$PSPSDK/lib -Wl,-q,-T$PSPSDK/lib/linkfile.prx -nostartfiles $(cat $DIR/file_list) $DIR/plugin.o -lstdc++ -lc -lpspkernel -lpspuser -lpspsdk -o $1.elf || exit 1
    
    echo "generating PRX $1"
    psp-fixup-imports $1.elf || exit 1
    psp-prxgen $1.elf $DIR/$1.prx || exit 1
}

function emit_info()  {
    # kernel prx
    #echo "PSP_MODULE_INFO($1, 0x1000, 1, 1);" > $DIR/include/music2_plugin_generated.h

    # user prx
    echo "PSP_MODULE_INFO($1, 0, 1, 1);" > $DIR/include/music2_plugin_generated.h

    echo "#define MAIN_THREAD \"$2\"" >> $DIR/include/music2_plugin_generated.h
    printf "" > $DIR/file_list
}

function build_plugin() {
    cd $DIR/plugins/$1 || exit 1
    emit_info music2_prx_plugin__$1 music2_prx_plugin__$1
    ./make.sh || exit 1
    link $1 || exit 1
    cd $DIR
    mv -v $1.prx seplugins/music2/plugins/$1.prx
}

