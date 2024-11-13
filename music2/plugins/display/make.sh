. $DIR/common.sh
dir=$1
shift 1

compile main
link_u $dir -lpspdisplay -lpspuser