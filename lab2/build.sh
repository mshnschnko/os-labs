BUILD_DIR="build"
BIN_DIR="."

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
cp $BUILD_DIR/host_mq $BIN_DIR
cp $BUILD_DIR/client_mq $BIN_DIR

rm -rf $BUILD_DIR
