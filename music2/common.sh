function compile() {
    echo "CC $1.c"
    psp-gcc -I $PSPSDK/include -I $DIR/include -DNOEXIT -DFPM_MIPS -O2 -G0 -Wall -fno-pic -c -o $1.o $1.c || exit 1
    printf -- "$1.o " >> $DIR/file_list
}

function link() {
    name=$1
    shift 1
    psp-build-exports -b $DIR/exports.exp > exports.c
    compile exports
    
    echo "linking $name.elf"
    psp-gcc -D_PSP_FW_VERSION=371 -L$DIR/lib -L$PSPSDK/lib -Wl,-q,-T$PSPSDK/lib/linkfile.prx -nostartfiles $(cat $DIR/file_list) $@ $(cat $DIR/before_libs) -lc -lpspkernel -lpspsysmem_kernel -lpspsysmem_user $(cat $DIR/after_libs) -lpspsdk -o $name.elf || exit 1
    
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

function emit_info()  {
    echo "#define MAIN_THREAD \"$2\"" > $DIR/include/music2_plugin_generated.h
    echo "PSP_MODULE_INFO(\"$1\", PSP_MODULE_KERNEL, 1, 1);" >> $DIR/include/music2_plugin_generated.h
    echo "PSP_NO_CREATE_MAIN_THREAD();" >> $DIR/include/music2_plugin_generated.h

    printf "" > $DIR/file_list
    printf "" > $DIR/before_libs
    printf "" > $DIR/after_libs

}

function build_plugin() {
    dir=$1
    name=$2
    shift 2
    cd $DIR/plugins/$dir || exit 1
    emit_info m2prx__$name m2prx__$name
    ./make.sh $dir $@ || exit 1
    cd $DIR
    mv -v $dir.prx seplugins/music2/plugins/$dir.prx
}

