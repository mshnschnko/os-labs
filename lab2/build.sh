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
cp $BUILD_DIR/host_shm $BIN_DIR
cp $BUILD_DIR/client_shm $BIN_DIR
cp $BUILD_DIR/host_mmap $BIN_DIR
cp $BUILD_DIR/client_mmap $BIN_DIR

rm -rf $BUILD_DIR
