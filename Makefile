## CONFIGURATION ##############################################################

ifeq ($(shell uname),Darwin)
  LUA_DIR := /usr/local
  LUA_LIBDIR := $(LUA_DIR)/lib/lua/5.1
  LUA_INCDIR := $(LUA_DIR)/include
  LUALIB := lua
else
  # Assuming Ubuntu
  LUA_LIBDIR := /usr/lib
  LUA_INCDIR := /usr/include/lua5.1
  LUALIB := lua5.1
endif

PROJECTNAME := luaamf

SONAME   := amf.so
ANAME    := lib$(PROJECTNAME).a
HNAME    := $(PROJECTNAME).h
TESTNAME := $(PROJECTNAME)-test
TESTLUA  := test.lua

LUA    := lua
CP     := cp
RM     := rm -f
RMDIR  := rm -df
MKDIR  := mkdir -p
CC     := gcc
LD     := gcc
AR     := ar rcu
RANLIB := ranlib
ECHO   := @echo
TOUCH  := touch

# Needed for tests only
CXX  := g++
LDXX := g++

OBJDIR := ./obj
TMPDIR := ./tmp
INCDIR := ./include
LIBDIR := ./lib

HFILE  := $(INCDIR)/$(HNAME)

CFLAGS  += -O2 -Wall -I$(LUA_INCDIR)
LDFLAGS += -L$(LUA_LIBDIR)

# Tested on OS X and Ubuntu
SOFLAGS :=
ifeq ($(shell uname),Darwin)
  SOFLAGS += -dynamiclib -undefined dynamic_lookup
else
  ifeq ($(shell uname),CYGWIN_NT-6.1)
    LDFLAGS += -llua
  else
    CFLAGS += -fPIC
  endif
  SOFLAGS += -shared
  LDFLAGS += -ldl
  RMDIR := rm -rf
endif

CFLAGS += $(MYCFLAGS)
LDFLAGS += $(MYLDFLAGS)

## MAIN TARGETS ###############################################################

all: $(LIBDIR)/$(SONAME) $(LIBDIR)/$(ANAME) $(HFILE)

clean: cleanlibs cleantest
	$(RM) $(HFILE)

install: $(LIBDIR)/$(SONAME)
	# Note header and static library are not copied anywhere
	$(CP) $(LIBDIR)/$(SONAME) $(LUA_LIBDIR)/$(SONAME)

$(HFILE):
	$(CP) src/$(HNAME) $(HFILE)

## GENERATED RELEASE TARGETS ##################################################

cleanlibs: cleanobjects
	$(RM) $(LIBDIR)/$(SONAME)
	$(RM) $(LIBDIR)/$(ANAME)

$(LIBDIR)/$(SONAME): $(OBJDIR)/decode.o $(OBJDIR)/encode.o $(OBJDIR)/load.o $(OBJDIR)/luaamf.o $(OBJDIR)/savebuffer.o $(OBJDIR)/save.o
	$(MKDIR) $(LIBDIR)
	$(LD) -o $@ $(OBJDIR)/decode.o $(OBJDIR)/encode.o $(OBJDIR)/load.o $(OBJDIR)/luaamf.o $(OBJDIR)/savebuffer.o $(OBJDIR)/save.o $(LDFLAGS) $(SOFLAGS)

$(LIBDIR)/$(ANAME): $(OBJDIR)/decode.o $(OBJDIR)/encode.o $(OBJDIR)/load.o $(OBJDIR)/luaamf.o $(OBJDIR)/savebuffer.o $(OBJDIR)/save.o
	$(MKDIR) $(LIBDIR)
	$(AR) $@ $(OBJDIR)/decode.o $(OBJDIR)/encode.o $(OBJDIR)/load.o $(OBJDIR)/luaamf.o $(OBJDIR)/savebuffer.o $(OBJDIR)/save.o
	$(RANLIB) $@

# objects:

cleanobjects:
	$(RM) $(OBJDIR)/decode.o $(OBJDIR)/encode.o $(OBJDIR)/load.o $(OBJDIR)/luaamf.o $(OBJDIR)/savebuffer.o $(OBJDIR)/save.o

$(OBJDIR)/decode.o: src/decode.c src/saveload.h src/decode.h
	$(CC) $(CFLAGS)  -o $@ -c src/decode.c

$(OBJDIR)/encode.o: src/encode.c src/luaheaders.h src/saveload.h \
 src/savebuffer.h src/encode.h
	$(CC) $(CFLAGS)  -o $@ -c src/encode.c

$(OBJDIR)/load.o: src/load.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/decode.h
	$(CC) $(CFLAGS)  -o $@ -c src/load.c

$(OBJDIR)/luaamf.o: src/luaamf.c src/luaheaders.h src/luaamf.h
	$(CC) $(CFLAGS)  -o $@ -c src/luaamf.c

$(OBJDIR)/savebuffer.o: src/savebuffer.c src/luaheaders.h \
 src/saveload.h src/savebuffer.h
	$(CC) $(CFLAGS)  -o $@ -c src/savebuffer.c

$(OBJDIR)/save.o: src/save.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/savebuffer.h src/encode.h
	$(CC) $(CFLAGS)  -o $@ -c src/save.c

## TEST TARGETS ###############################################################

test: testc89 testc99 testc++98
	$(ECHO) "===== TESTS PASSED ====="

resettest: resettestc89 resettestc99 resettestc++98

cleantest: cleantestc89 cleantestc99 cleantestc++98

## GENERATED TEST TARGETS #####################################################

## ----- Begin c89 -----

testc89: lua-testsc89 c-testsc89

lua-testsc89: $(TMPDIR)/c89/.luatestspassed

c-testsc89: $(TMPDIR)/c89/.ctestspassed

$(TMPDIR)/c89/.luatestspassed: $(TMPDIR)/c89/$(SONAME) test/$(TESTLUA)
	$(ECHO) "===== Running Lua tests for c89 ====="
	@$(LUA) \
		-e "package.cpath='$(TMPDIR)/c89/$(SONAME);'..package.cpath" \
		test/$(TESTLUA)
	$(TOUCH) $(TMPDIR)/c89/.luatestspassed
	$(ECHO) "===== Lua tests for c89 PASSED ====="

$(TMPDIR)/c89/.ctestspassed: $(TMPDIR)/c89/$(TESTNAME) test/$(TESTLUA)
	$(ECHO) "===== Running C tests for c89 ====="
	$(TMPDIR)/c89/$(TESTNAME)
	$(TOUCH) $(TMPDIR)/c89/.ctestspassed
	$(ECHO) "===== C tests for c89 PASSED ====="

$(TMPDIR)/c89/$(TESTNAME): $(OBJDIR)/c89-test.o $(TMPDIR)/c89/$(ANAME)
	$(MKDIR) $(TMPDIR)/c89
	$(LD) -o $@ $(OBJDIR)/c89-test.o $(LDFLAGS) -lm -l$(LUALIB) -l$(PROJECTNAME) -L$(TMPDIR)/c89

resettestc89:
	$(RM) $(TMPDIR)/c89/.luatestspassed
	$(RM) $(TMPDIR)/c89/.ctestspassed

cleantestc89: cleanlibsc89 resettestc89 \
                    cleantestobjectsc89
	$(RM) $(TMPDIR)/c89/$(TESTNAME)
	$(RMDIR) $(TMPDIR)/c89

# testobjectsc89:

cleantestobjectsc89:
	$(RM) $(OBJDIR)/c89-test.o

$(OBJDIR)/c89-test.o: test/test.c test/test.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c89 -Isrc/ -o $@ -c test/test.c

cleanlibsc89: cleanobjectsc89
	$(RM) $(TMPDIR)/c89/$(SONAME)
	$(RM) $(TMPDIR)/c89/$(ANAME)

$(TMPDIR)/c89/$(SONAME): $(OBJDIR)/c89-decode.o $(OBJDIR)/c89-encode.o $(OBJDIR)/c89-load.o $(OBJDIR)/c89-luaamf.o $(OBJDIR)/c89-savebuffer.o $(OBJDIR)/c89-save.o
	$(MKDIR) $(TMPDIR)/c89
	$(LD) -o $@ $(OBJDIR)/c89-decode.o $(OBJDIR)/c89-encode.o $(OBJDIR)/c89-load.o $(OBJDIR)/c89-luaamf.o $(OBJDIR)/c89-savebuffer.o $(OBJDIR)/c89-save.o $(LDFLAGS) $(SOFLAGS)

$(TMPDIR)/c89/$(ANAME): $(OBJDIR)/c89-decode.o $(OBJDIR)/c89-encode.o $(OBJDIR)/c89-load.o $(OBJDIR)/c89-luaamf.o $(OBJDIR)/c89-savebuffer.o $(OBJDIR)/c89-save.o
	$(MKDIR) $(TMPDIR)/c89
	$(AR) $@ $(OBJDIR)/c89-decode.o $(OBJDIR)/c89-encode.o $(OBJDIR)/c89-load.o $(OBJDIR)/c89-luaamf.o $(OBJDIR)/c89-savebuffer.o $(OBJDIR)/c89-save.o
	$(RANLIB) $@

# objectsc89:

cleanobjectsc89:
	$(RM) $(OBJDIR)/c89-decode.o $(OBJDIR)/c89-encode.o $(OBJDIR)/c89-load.o $(OBJDIR)/c89-luaamf.o $(OBJDIR)/c89-savebuffer.o $(OBJDIR)/c89-save.o

$(OBJDIR)/c89-decode.o: src/decode.c src/saveload.h src/decode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c89 -o $@ -c src/decode.c

$(OBJDIR)/c89-encode.o: src/encode.c src/luaheaders.h src/saveload.h \
 src/savebuffer.h src/encode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c89 -o $@ -c src/encode.c

$(OBJDIR)/c89-load.o: src/load.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/decode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c89 -o $@ -c src/load.c

$(OBJDIR)/c89-luaamf.o: src/luaamf.c src/luaheaders.h src/luaamf.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c89 -o $@ -c src/luaamf.c

$(OBJDIR)/c89-savebuffer.o: src/savebuffer.c src/luaheaders.h \
 src/saveload.h src/savebuffer.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c89 -o $@ -c src/savebuffer.c

$(OBJDIR)/c89-save.o: src/save.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/savebuffer.h src/encode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c89 -o $@ -c src/save.c

## ----- Begin c99 -----

testc99: lua-testsc99 c-testsc99

lua-testsc99: $(TMPDIR)/c99/.luatestspassed

c-testsc99: $(TMPDIR)/c99/.ctestspassed

$(TMPDIR)/c99/.luatestspassed: $(TMPDIR)/c99/$(SONAME) test/$(TESTLUA)
	$(ECHO) "===== Running Lua tests for c99 ====="
	@$(LUA) \
		-e "package.cpath='$(TMPDIR)/c99/$(SONAME);'..package.cpath" \
		test/$(TESTLUA)
	$(TOUCH) $(TMPDIR)/c99/.luatestspassed
	$(ECHO) "===== Lua tests for c99 PASSED ====="

$(TMPDIR)/c99/.ctestspassed: $(TMPDIR)/c99/$(TESTNAME) test/$(TESTLUA)
	$(ECHO) "===== Running C tests for c99 ====="
	$(TMPDIR)/c99/$(TESTNAME)
	$(TOUCH) $(TMPDIR)/c99/.ctestspassed
	$(ECHO) "===== C tests for c99 PASSED ====="

$(TMPDIR)/c99/$(TESTNAME): $(OBJDIR)/c99-test.o $(TMPDIR)/c99/$(ANAME)
	$(MKDIR) $(TMPDIR)/c99
	$(LD) -o $@ $(OBJDIR)/c99-test.o $(LDFLAGS) -lm -l$(LUALIB) -l$(PROJECTNAME) -L$(TMPDIR)/c99

resettestc99:
	$(RM) $(TMPDIR)/c99/.luatestspassed
	$(RM) $(TMPDIR)/c99/.ctestspassed

cleantestc99: cleanlibsc99 resettestc99 \
                    cleantestobjectsc99
	$(RM) $(TMPDIR)/c99/$(TESTNAME)
	$(RMDIR) $(TMPDIR)/c99

# testobjectsc99:

cleantestobjectsc99:
	$(RM) $(OBJDIR)/c99-test.o

$(OBJDIR)/c99-test.o: test/test.c test/test.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c99 -Isrc/ -o $@ -c test/test.c

cleanlibsc99: cleanobjectsc99
	$(RM) $(TMPDIR)/c99/$(SONAME)
	$(RM) $(TMPDIR)/c99/$(ANAME)

$(TMPDIR)/c99/$(SONAME): $(OBJDIR)/c99-decode.o $(OBJDIR)/c99-encode.o $(OBJDIR)/c99-load.o $(OBJDIR)/c99-luaamf.o $(OBJDIR)/c99-savebuffer.o $(OBJDIR)/c99-save.o
	$(MKDIR) $(TMPDIR)/c99
	$(LD) -o $@ $(OBJDIR)/c99-decode.o $(OBJDIR)/c99-encode.o $(OBJDIR)/c99-load.o $(OBJDIR)/c99-luaamf.o $(OBJDIR)/c99-savebuffer.o $(OBJDIR)/c99-save.o $(LDFLAGS) $(SOFLAGS)

$(TMPDIR)/c99/$(ANAME): $(OBJDIR)/c99-decode.o $(OBJDIR)/c99-encode.o $(OBJDIR)/c99-load.o $(OBJDIR)/c99-luaamf.o $(OBJDIR)/c99-savebuffer.o $(OBJDIR)/c99-save.o
	$(MKDIR) $(TMPDIR)/c99
	$(AR) $@ $(OBJDIR)/c99-decode.o $(OBJDIR)/c99-encode.o $(OBJDIR)/c99-load.o $(OBJDIR)/c99-luaamf.o $(OBJDIR)/c99-savebuffer.o $(OBJDIR)/c99-save.o
	$(RANLIB) $@

# objectsc99:

cleanobjectsc99:
	$(RM) $(OBJDIR)/c99-decode.o $(OBJDIR)/c99-encode.o $(OBJDIR)/c99-load.o $(OBJDIR)/c99-luaamf.o $(OBJDIR)/c99-savebuffer.o $(OBJDIR)/c99-save.o

$(OBJDIR)/c99-decode.o: src/decode.c src/saveload.h src/decode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c99 -o $@ -c src/decode.c

$(OBJDIR)/c99-encode.o: src/encode.c src/luaheaders.h src/saveload.h \
 src/savebuffer.h src/encode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c99 -o $@ -c src/encode.c

$(OBJDIR)/c99-load.o: src/load.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/decode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c99 -o $@ -c src/load.c

$(OBJDIR)/c99-luaamf.o: src/luaamf.c src/luaheaders.h src/luaamf.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c99 -o $@ -c src/luaamf.c

$(OBJDIR)/c99-savebuffer.o: src/savebuffer.c src/luaheaders.h \
 src/saveload.h src/savebuffer.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c99 -o $@ -c src/savebuffer.c

$(OBJDIR)/c99-save.o: src/save.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/savebuffer.h src/encode.h
	$(CC) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c -std=c99 -o $@ -c src/save.c

## ----- Begin c++98 -----

testc++98: lua-testsc++98 c-testsc++98

lua-testsc++98: $(TMPDIR)/c++98/.luatestspassed

c-testsc++98: $(TMPDIR)/c++98/.ctestspassed

$(TMPDIR)/c++98/.luatestspassed: $(TMPDIR)/c++98/$(SONAME) test/$(TESTLUA)
	$(ECHO) "===== Running Lua tests for c++98 ====="
	@$(LUA) \
		-e "package.cpath='$(TMPDIR)/c++98/$(SONAME);'..package.cpath" \
		test/$(TESTLUA)
	$(TOUCH) $(TMPDIR)/c++98/.luatestspassed
	$(ECHO) "===== Lua tests for c++98 PASSED ====="

$(TMPDIR)/c++98/.ctestspassed: $(TMPDIR)/c++98/$(TESTNAME) test/$(TESTLUA)
	$(ECHO) "===== Running C tests for c++98 ====="
	$(TMPDIR)/c++98/$(TESTNAME)
	$(TOUCH) $(TMPDIR)/c++98/.ctestspassed
	$(ECHO) "===== C tests for c++98 PASSED ====="

$(TMPDIR)/c++98/$(TESTNAME): $(OBJDIR)/c++98-test.o $(TMPDIR)/c++98/$(ANAME)
	$(MKDIR) $(TMPDIR)/c++98
	$(LDXX) -o $@ $(OBJDIR)/c++98-test.o $(LDFLAGS) -lm -l$(LUALIB) -l$(PROJECTNAME) -L$(TMPDIR)/c++98

resettestc++98:
	$(RM) $(TMPDIR)/c++98/.luatestspassed
	$(RM) $(TMPDIR)/c++98/.ctestspassed

cleantestc++98: cleanlibsc++98 resettestc++98 \
                    cleantestobjectsc++98
	$(RM) $(TMPDIR)/c++98/$(TESTNAME)
	$(RMDIR) $(TMPDIR)/c++98

# testobjectsc++98:

cleantestobjectsc++98:
	$(RM) $(OBJDIR)/c++98-test.o

$(OBJDIR)/c++98-test.o: test/test.c test/test.h
	$(CXX) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c++ -std=c++98 -Isrc/ -o $@ -c test/test.c

cleanlibsc++98: cleanobjectsc++98
	$(RM) $(TMPDIR)/c++98/$(SONAME)
	$(RM) $(TMPDIR)/c++98/$(ANAME)

$(TMPDIR)/c++98/$(SONAME): $(OBJDIR)/c++98-decode.o $(OBJDIR)/c++98-encode.o $(OBJDIR)/c++98-load.o $(OBJDIR)/c++98-luaamf.o $(OBJDIR)/c++98-savebuffer.o $(OBJDIR)/c++98-save.o
	$(MKDIR) $(TMPDIR)/c++98
	$(LDXX) -o $@ $(OBJDIR)/c++98-decode.o $(OBJDIR)/c++98-encode.o $(OBJDIR)/c++98-load.o $(OBJDIR)/c++98-luaamf.o $(OBJDIR)/c++98-savebuffer.o $(OBJDIR)/c++98-save.o $(LDFLAGS) $(SOFLAGS)

$(TMPDIR)/c++98/$(ANAME): $(OBJDIR)/c++98-decode.o $(OBJDIR)/c++98-encode.o $(OBJDIR)/c++98-load.o $(OBJDIR)/c++98-luaamf.o $(OBJDIR)/c++98-savebuffer.o $(OBJDIR)/c++98-save.o
	$(MKDIR) $(TMPDIR)/c++98
	$(AR) $@ $(OBJDIR)/c++98-decode.o $(OBJDIR)/c++98-encode.o $(OBJDIR)/c++98-load.o $(OBJDIR)/c++98-luaamf.o $(OBJDIR)/c++98-savebuffer.o $(OBJDIR)/c++98-save.o
	$(RANLIB) $@

# objectsc++98:

cleanobjectsc++98:
	$(RM) $(OBJDIR)/c++98-decode.o $(OBJDIR)/c++98-encode.o $(OBJDIR)/c++98-load.o $(OBJDIR)/c++98-luaamf.o $(OBJDIR)/c++98-savebuffer.o $(OBJDIR)/c++98-save.o

$(OBJDIR)/c++98-decode.o: src/decode.c src/saveload.h src/decode.h
	$(CXX) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c++ -std=c++98 -o $@ -c src/decode.c

$(OBJDIR)/c++98-encode.o: src/encode.c src/luaheaders.h src/saveload.h \
 src/savebuffer.h src/encode.h
	$(CXX) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c++ -std=c++98 -o $@ -c src/encode.c

$(OBJDIR)/c++98-load.o: src/load.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/decode.h
	$(CXX) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c++ -std=c++98 -o $@ -c src/load.c

$(OBJDIR)/c++98-luaamf.o: src/luaamf.c src/luaheaders.h src/luaamf.h
	$(CXX) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c++ -std=c++98 -o $@ -c src/luaamf.c

$(OBJDIR)/c++98-savebuffer.o: src/savebuffer.c src/luaheaders.h \
 src/saveload.h src/savebuffer.h
	$(CXX) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c++ -std=c++98 -o $@ -c src/savebuffer.c

$(OBJDIR)/c++98-save.o: src/save.c src/luaheaders.h src/luaamf.h \
 src/saveload.h src/savebuffer.h src/encode.h
	$(CXX) $(CFLAGS) -Werror -Wall -Wextra -pedantic -x c++ -std=c++98 -o $@ -c src/save.c

## END OF GENERATED TARGETS ###################################################

.PHONY: all clean install cleanlibs cleanobjects test resettest cleantest testc89 lua-testsc89 c-testsc89 resettestc89 cleantestc89 cleantestobjectsc89 cleanlibsc89 cleanobjectsc89 testc99 lua-testsc99 c-testsc99 resettestc99 cleantestc99 cleantestobjectsc99 cleanlibsc99 cleanobjectsc99 testc++98 lua-testsc++98 c-testsc++98 resettestc++98 cleantestc++98 cleantestobjectsc++98 cleanlibsc++98 cleanobjectsc++98
