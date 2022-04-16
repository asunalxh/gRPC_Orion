rm -f *.pb.*

protoc -I . --cpp_out=. crypto.proto
g++ -std=c++11 `pkg-config --cflags protobuf grpc`  -c -o crypto.pb.o crypto.pb.cc
protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` crypto.proto
g++ -std=c++11 `pkg-config --cflags protobuf grpc`  -c -o crypto.grpc.pb.o crypto.grpc.pb.cc