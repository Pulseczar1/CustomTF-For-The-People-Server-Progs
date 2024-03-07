ENGINE_DIR=$(PROGS_TOPLEVEL)/../ServerEngine
CLIENT_DIR=$(PROGS_TOPLEVEL)/../ServerEngine/cpqw/client

# PZ: I've converted the server code from C to C++. So, all the 'CXXFLAGS' below were 'CFLAGS'.
# TODO: Add a condition in here for compiling the original QuakeC VM pr_cmds.cpp (now called 'pr_cmds_vm.cpp).
#       That also needs to include pr_exec.cpp. Condition should be based on precompiler definition of "USE_PROGS_VM".

# WARNING: If you want to check for comparison of float values, use -Wfloat-equal. Unfortunately, I think it reports
# too many things, like comparison of a float to a whole number. However, due to rounding errors, using == and !=
# with float values is dangerous (0.6 might be represented as 0.600000024 in memory). This is a problem with QuakeC
# code ported to native languages (programs, really) like C and C++.

# Test version with debug symbols
ifeq ($(CFG),test)
  $(info Configuring compilation for the Debug configuration)
  CXXFLAGS=-std=c++11 -m32 -Wall -g -I$(ENGINE_DIR) -I$(CLIENT_DIR) -Wno-trigraphs
  LDFLAGS=-std=c++11 -m32 -g -L$(ENGINE_DIR) -L$(CLIENT_DIR) -lm
  SHAREDOBJECT=4TP_Progs.so

# Test version with debug symbols, that also executes C++ debug libraries, that perform extra error checking
# WARNING: AVOID USING THIS ONE FOR AN EXECUTABLE INTENDED FOR THE LIVE, PUBLIC SERVER. -D_GLIBCXX_DEBUG PUTS IN EXTRA
# ERROR CHECKING, THAT MAY AFFECT PERFORMANCE.
else ifeq ($(CFG),test_with_debug_libs)
  $(info Configuring compilation for the Debug configuration, with debug C++ libraries)
  CXXFLAGS=-std=c++11 -D_GLIBCXX_DEBUG -m32 -Wall -g -I$(ENGINE_DIR) -I$(CLIENT_DIR) -Wno-trigraphs
  LDFLAGS=-std=c++11 -D_GLIBCXX_DEBUG -m32 -g -L$(ENGINE_DIR) -L$(CLIENT_DIR) -lm
  SHAREDOBJECT=4TP_Progs.so

# Release version, containing no debug symbols (no CFG option set; just entered 'make')
else
  $(info Configuring compilation for the Release configuration)
  #-fomit-frame-pointer -fno-strength-reduce -O6
  #crashes on all O levels, 1 to 6
  CXXFLAGS=-std=c++11 -m32 -Wall -I$(ENGINE_DIR) -I$(CLIENT_DIR) -Wno-trigraphs
  LDFLAGS=-std=c++11 -m32 -L$(ENGINE_DIR) -L$(CLIENT_DIR) -lm
  SHAREDOBJECT=4TP_Progs.so
endif
