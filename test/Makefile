# arguments
platform = centos



# paths home
project_home       = .
bin_dir            = $(project_home)/bin/$(platform)
object_dir         = $(project_home)/.objs
gnu_home           = $(project_home)/../../gnu_libs
bson_home          = $(gnu_home)/bson
mongoc_home        = $(gnu_home)/mongoc
openssl_home       = $(gnu_home)/openssl
libmongo_home      = ..



# includes of bson headers
bson_inc_path      = $(bson_home)/inc
bson_includes      = -I$(bson_inc_path)

# includes of mongoc headers
mongoc_inc_path    = $(mongoc_home)/inc
mongoc_includes    = -I$(mongoc_inc_path)

# includes of libmongo headers
libmongo_inc_path  = $(libmongo_home)/inc
libmongo_includes  = -I$(libmongo_inc_path)


# all includes that local solution needs
includes           = $(bson_includes)
includes          += $(mongoc_includes)
includes          += $(libmongo_includes)



# source files of local solution
local_src_path     = $(project_home)
local_source       = $(filter %.cpp, $(shell find $(local_src_path) -depth -name "*.cpp"))



# objects of local solution
local_objects      = $(local_source:$(project_home)%.cpp=$(object_dir)%.o)



# system libraries
system_libs        = -pthread -ldl -lresolv -lrt

# system libraries
ifeq ($(platform), centos)
	system_libs   += -lz
endif

# openssl libraries
openssl_lib_inc    = $(openssl_home)/lib/$(platform)
openssl_libs       = -L$(openssl_lib_inc) -lssl -lcrypto

# bson libraries
bson_lib_inc       = $(bson_home)/lib/$(platform)
bson_libs          = -L$(bson_lib_inc) -lbson-static-1.0

# mongoc libraries
mongoc_lib_inc     = $(mongoc_home)/lib/$(platform)
mongoc_libs        = -L$(mongoc_lib_inc) -lmongoc-static-1.0

# libmongo libraries
libmongo_lib_inc   = $(libmongo_home)/lib/$(platform)
libmongo_libs      = -L$(libmongo_lib_inc) -lmongo

# local depends libraries
depend_libs        = $(libmongo_libs)
depend_libs       += $(mongoc_libs)
depend_libs       += $(bson_libs)
depend_libs       += $(openssl_libs)
depend_libs       += $(system_libs)



# output execution
output_exec        = $(bin_dir)/test



# my g++ not support nullptr and 64bits
# so no -m64, and add a macro -Dnullptr=0

# build flags for objects
build_obj_flags_d  = -std=c++11 -g -Wall -O1 -pipe -fPIC -Dnullptr=0
build_obj_flags_r  = -std=c++11 -Wall -O3 -pipe -fPIC
build_obj_flags    = $(build_obj_flags_r)

# build flags for execution
build_exec_flags   = $(build_obj_flags)



# build targets

# let 'build' be default target, build all targets
build    : $(local_objects)
	mkdir -p $(bin_dir)
	@echo
	@echo "@@@@@  start making test  @@@@@"
	g++ $(build_exec_flags) -o $(output_exec) $^ $(depend_libs)
	@echo "@@@@@  make test success  @@@@@"
	@echo

cpfile   :
	@cp $(boost_web_lib_inc)/* $(bin_dir)/

run      : cpfile
	@export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(bin_dir) && $(output_exec)
# it same as: @env LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$(bin_dir) $(output_exec)

# build all objects
$(object_dir)/%.o:$(project_home)/%.cpp
	@dir=`dirname $@`;      \
    if [ ! -d $$dir ]; then \
        mkdir -p $$dir;     \
    fi
	g++ -c $(build_obj_flags) $(includes) -o $@ $<

clean    :
	rm -rf $(object_dir) $(output_exec)

rebuild  : clean build
