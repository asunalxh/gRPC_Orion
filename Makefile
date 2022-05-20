CXXFLAGS := -std=c++17

######## gRPC Settings ########
HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)

ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -pthread\
           -lgrpc++_reflection\
           -ldl
else
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -pthread\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl
endif
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

GRPC_LINK_FLAGS := -lssl -lcrypto

PROTOS_PATH = common/

PROTO_FILES = $(wildcard $(PROTOS_PATH)/*.proto)

PB_CC_FILES = $(PROTO_FILES:.proto=.pb.cc)
GRPC_PB_CC_FILES = $(PROTO_FILES:.proto=.grpc.pb.cc)

PROTO_OBJECT_FILES = $(PB_CC_FILES:.cc=.o) $(GRPC_PB_CC_FILES:.cc=.o) 

######## App Settings ########
Target := testApp

Client_App_Files := $(wildcard Client/*.cpp)
Server_App_Files := $(wildcard Server/*.cpp)
Common_App_Files := $(wildcard common/*.cpp)

Client_Object_Files := $(Client_App_Files:.cpp=.o)
Server_Object_Files := $(Server_App_Files:.cpp=.o)
Common_Object_Files := $(Common_App_Files:.cpp=.o)

Link_Flags = $(LDFLAGS) $(GRPC_LINK_FLAGS)

.PHONY: all clean

all: $(Target)

$(PROTOS_PATH)/%.grpc.pb.cc: $(PROTOS_PATH)/%.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=$(PROTOS_PATH) --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

$(PROTOS_PATH)/%.pb.cc: $(PROTOS_PATH)/%.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=$(PROTOS_PATH) $<

common/%.o: common/%.cpp
	$(CXX) $(CXXFLAGS)  -c $< -o $@

Client/%.o: Client/%.cpp
	$(CXX) $(CXXFLAGS)  -c $< -o $@

Server/%.o: Server/%.cpp
	$(CXX) $(CXXFLAGS)  -c $< -o $@

$(Target): $(Common_Object_Files) $(PROTO_OBJECT_FILES) $(Server_Object_Files) $(Client_Object_Files)
	$(CXX) $(CXXFLAGS) $^ $(Link_Flags) -o $@


clean:
	@rm -f common/*.o common/*.pb.cc common/*.pb.h
	@rm -f Client/*.o Server/*.o
	@rm -f $(Target)
