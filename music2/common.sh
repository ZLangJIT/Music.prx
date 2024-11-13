function compile_c() {
    echo "CC $1.c"
    psp-gcc -I $PSPSDK/include -I $DIR/include -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic -c -o $1.o $1.c || exit 1
    printf -- "$1.o " >> $DIR/file_list
}

function compile() {
    echo "CXX $1.cpp"
    psp-g++ -I $PSPSDK/include -I $DIR/include -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic -fno-exceptions -fno-rtti -c -o $1.o $1.cpp || exit 1
    printf --  "$1.o " >> $DIR/file_list
}

function link_k() {
    name=$1
    shift 1
    echo "GEN exports.c"
    psp-build-exports -b $DIR/exports.exp > exports.c
    compile_c exports
    
    echo "link $name"
    echo "link $name before libs '$(cat $DIR/before_libs)'"
    echo "link $name after libs '$(cat $DIR/after_libs)'"
    psp-gcc -D_PSP_FW_VERSION=371 -L$DIR/lib -L$PSPSDK/lib -Wl,-q,-T$PSPSDK/lib/linkfile.prx -nostartfiles $(cat $DIR/file_list) $@ $(cat $DIR/before_libs) -lc -lpspkernel $(cat $DIR/after_libs) -lpspsdk -o $name.elf || exit 1
    
    echo "GEN $name.prx"
    psp-fixup-imports $name.elf || exit 1
    psp-prxgen $name.elf $DIR/$name.prx || exit 1
}

function link_before() {
    printf -- "$@" >> $DIR/before_libs
    printf " " >> $DIR/before_libs
}

function link_after() {
    printf -- "$@" >> $DIR/after_libs
    printf " " >> $DIR/after_libs
}

function link_u() {
    name=$1
    shift 1
    link_before -lstdc++
    link_after -lpspuser
    link_k $name $DIR/plugin.o
}

function emit_info_base()  {
    echo "#define MAIN_THREAD \"$2\"" > $DIR/include/music2_plugin_generated.h
    printf "" > $DIR/file_list
    printf "" > $DIR/before_libs
    printf "" > $DIR/after_libs
}

function emit_info_u()  {
    emit_info_base $@
    
    # user prx
    echo "PSP_MODULE_INFO($1, 0, 1, 1);" >> $DIR/include/music2_plugin_generated.h
}

function emit_info_k()  {
    emit_info_base $@
    
    # kernel prx
    echo "PSP_MODULE_INFO(\"$1\", 0x1000, 1, 1);" >> $DIR/include/music2_plugin_generated.h
}

function build_plugin() {
    dir=$1
    name=$2
    shift 2
    cd $DIR/plugins/$dir || exit 1
    emit_info_u m2prx__$name m2prx__$name
    ./make.sh $dir $@ || exit 1
    cd $DIR
    mv -v $dir.prx seplugins/music2/plugins/$dir.prx
}

