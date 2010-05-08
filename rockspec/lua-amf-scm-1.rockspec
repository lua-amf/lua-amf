package = "lua-amf"
version = "scm-1"
source = {
   url = "git://github.com/lua-amf/lua-amf.git"
}
description = {
   summary = "A library to work with AMF format",
   detailed = [[
      The library currently allows user to save simple Lua data to AMF.
   ]],
   homepage = "http://github.com/lua-amf/lua-amf",
   license = "MIT/X11"
}
dependencies = {
   "lua >= 5.1"
}
build = {
   type = "builtin",
   modules = {
      lua-amf = {
         sources = {
            "src/load.c",
            "src/save.c",
            "src/encode.c",
            "src/decode.c",
            "src/luaamf.c",
            "src/lualess.c",
            "src/savebuffer.c",
         },
         incdirs = {
            "src/"
         }
      }
   }
}
