# arguments
platform = centos



project_home       = ..
build_dir          = $(project_home)/sln
bin_dir            = $(project_home)/lib/$(platform)
object_dir         = $(project_home)/.objs
gnu_libs_home      = $(project_home)/../gnu_libs
bson_home          = $(gnu_libs_home)/bson
mongoc_home        = $(gnu_libs_home)/mongoc



# includes of bson headers
bson_inc_path      = $(bson_home)/inc
bson_includes      = -I$(bson_inc_path)

# includes of mongoc headers
mongoc_inc_path   = $(mongoc_home)/inc
mongoc_includes   = -I$(mongoc_inc_path)

# includes of libmongo headers
libmongo_inc_path  = $(project_home)/inc
libmongo_includes  = -I$(libmongo_inc_path)



# all includes that libmongo solution needs
includes           = $(bson_includes)
includes          += $(mongoc_includes)
includes          += $(libmongo_includes)



# source files of libmongo solution

# source files of libmongo project
libmongo_src_path  = $(project_home)/src
libmongo_source    = $(filter %.cpp, $(shell find $(libmongo_src_path) -depth -name "*.cpp"))



# objects of libmongo solution
libmongo_objects   = $(libmongo_source:$(project_home)%.cpp=$(object_dir)%.o)



# output librarys
libmongo_output_lib= $(bin_dir)/libmongo.a



# build flags for objects
build_obj_flags    = -std=c++11 -g -Wall -O1 -pipe -fPIC

# build flags for execution
build_exec_flags   = $(build_obj_flags)

# build flags for dynamic link librarys
build_so_flags     = -std=c++11 -shared

# build flags for static link librarys
build_a_flags      = -rv



# build targets
targets            = mongo

# let 'build' be default target, build all targets
build    : $(targets)

mongo    : $(libmongo_objects)
	mkdir -p $(bin_dir)
	@echo
	@echo "@@@@@  start making libmongo  @@@@@"
	ar $(build_a_flags) $(libmongo_output_lib) $^
	@echo "@@@@@  make libmongo success  @@@@@"
	@echo

# build all objects
$(object_dir)/%.o:$(project_home)/%.cpp
	@dir=`dirname $@`;      \
    if [ ! -d $$dir ]; then \
        mkdir -p $$dir;     \
    fi
	g++ -c $(build_obj_flags) $(includes) -o $@ $<

clean    :
	rm -rf $(object_dir) $(bin_dir)/lib*

rebuild  : clean build
