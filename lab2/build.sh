BUILD_DIR="build"
BIN_DIR="bin"

mkdir $BUILD_DIR
cd $BUILD_DIR
cmake ..
make
cd ..
[ ! -d $BIN_DIR ] && mkdir $BIN_DIR
cp $BUILD_DIR/host_fifo $BIN_DIR
cp $BUILD_DIR/client_fifo $BIN_DIR

rm -rf $BUILD_DIR
