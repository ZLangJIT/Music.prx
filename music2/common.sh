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

function link_u() {
    echo "GEN EXPORTS $1"
    psp-build-exports -b $DIR/exports.exp > exports.c
    compile_c exports
    
    echo "link ELF $1"
    psp-gcc -D_PSP_FW_VERSION=371 -L$DIR/lib -L$PSPSDK/lib -Wl,-q,-T$PSPSDK/lib/linkfile.prx -nostartfiles $(cat $DIR/file_list) $DIR/plugin.o $DIR/plugin_main.o -lstdc++ -lc -lpspkernel -lpspuser -lpspsdk -o $1.elf || exit 1
    
    echo "generating PRX $1"
    psp-fixup-imports $1.elf || exit 1
    psp-prxgen $1.elf $DIR/$1.prx || exit 1
}

function link_k() {
    echo "GEN EXPORTS $1"
    psp-build-exports -b $DIR/exports.exp > exports.c
    compile_c exports
    
    echo "link ELF $1"
    psp-gcc -D_PSP_FW_VERSION=371 -L$DIR/lib -L$PSPSDK/lib -Wl,-q,-T$PSPSDK/lib/linkfile.prx -nostartfiles $(cat $DIR/file_list) $DIR/plugin_main.o -lc -lpspkernel -lpspsdk -o $1.elf || exit 1
    
    echo "generating PRX $1"
    psp-fixup-imports $1.elf || exit 1
    psp-prxgen $1.elf $DIR/$1.prx || exit 1
}

function emit_info_base()  {
    echo "#define MAIN_THREAD \"$2\"" > $DIR/include/music2_plugin_generated.h
    printf "" > $DIR/file_list
}

function emit_info_u()  {
    emit_info_base $@
    
    # user prx
    echo "PSP_MODULE_INFO(\"$1\", 0, 1, 1);" >> $DIR/include/music2_plugin_generated.h
}

function emit_info_k()  {
    emit_info_base $@
    
    # kernel prx
    echo "PSP_MODULE_INFO(\"$1\", 0x1000, 1, 1);" >> $DIR/include/music2_plugin_generated.h
}

function build_plugin() {
    cd $DIR/plugins/$1 || exit 1
    emit_info_u music2_prx_plugin__$1 music2_prx_plugin__$1
    ./make.sh || exit 1
    link_u $1 || exit 1
    cd $DIR
    mv -v $1.prx seplugins/music2/plugins/$1.prx
}

