BUILD_DIR="build"
BIN_DIR="."

mkdir $BUILD_DIR
cd $BUILD_DIR
cmake ..
make
cd ..
[ ! -d $BIN_DIR ] && mkdir $BIN_DIR
cp $BUILD_DIR/host_* $BIN_DIR
cp $BUILD_DIR/client_* $BIN_DIR

rm -rf $BUILD_DIR
