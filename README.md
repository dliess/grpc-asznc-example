```bash
#Prerequisites
sudo pacman -S make cmake autoconf libtool pkg-config 
git clone --recurse-submodules -b v1.66.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
cd cmake/build
cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$REPO_DIR/grpc_install -B.   -S../..
make -j8
make install
cd $REPO_DIR

grpc_install/bin/protoc -I=. --cpp_out=./generated --grpc_out=./generated --plugin=protoc-gen-grpc=./grpc_install/bin/grpc_cpp_plugin ./simple.proto

```
