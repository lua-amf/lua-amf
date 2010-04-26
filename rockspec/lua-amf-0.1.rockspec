package = "lua-amf"
version = "0.1"
source = {
   url = "git://github.com/vladfedin/lua-amf.git"
}
description = {
   summary = "A library to work with AMF format",
   detailed = [[
      This library is still in its embryonic phase. An appropriate description would be added later.
   ]],
   homepage = "http://github.com/vladfedin/lua-amf",
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
