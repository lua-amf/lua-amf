-- 0010-simple.lua: Lua-amf test suite
-- This file is a part of lua-amf library
-- Copyright (c) lua-amf authors (see file `COPYRIGHT` for the license)

package.cpath = "../lib/?.so;lib/?.so;"

local luaamf_local = assert(require('amf'))
local print, require, assert = print, require, assert

assert(pcall(dofile, 'lua-nucleo/strict.lua'), "lua-nucleo not found.")
dofile('lua-nucleo/import.lua')

local make_suite = select(1, ...)
assert(type(make_suite) == "function")
local test = make_suite("luaamf", { })

local test_data = assert(dofile('test/data/AMF_generated.lua'))

-- Utility functions

local ensure_equals = import 'lua-nucleo/ensure.lua' { 'ensure_equals' }
local deepequals = import 'lua-nucleo/tdeepequals.lua' { 'tdeepequals' }

local escape_string = function(str)
  return str:gsub(
      "[^0-9A-Za-z_%- :]",
      function(c)
        return ("\\%03u"):format(c:byte())
      end
    )
end

-- Test helper functions

local check_loaded_ok = function(eq, saved, data)
  assert(eq(data, luaamf_local.load(saved)), "Not equal.")
  return saved
end

local check_fn_ok = function(eq, ...)
  local saved = assert(luaamf_local.save(...))
  assert(type(saved) == "string")
  print("saved length", #saved, "(display truncated to 70 chars)")
  print(escape_string(saved):sub(1, 70))
  return check_loaded_ok(eq, saved, ...)
end

local check_ok = function(...)
  print("check_ok")
  return check_fn_ok(deepequals, ...)
end

local check_load_ok = function(data)
  local loaded = luaamf_local.load(data)
  print("  Loaded:", loaded, "(type: " .. type(loaded) .. ")")
  local saved = luaamf_local.save(loaded)
  -- this test wont pass due to integer <--> number <--> double decoding
  -- assert(deepequals(data, saved), "Not equal.")
  local reloaded = luaamf_local.load(saved)
  print("Reloaded:", reloaded, "(type: " .. type(reloaded) .. ")")
  if loaded == loaded then
    assert(deepequals(loaded, reloaded), "Not equal.")
  else
    assert(loaded ~= loaded and reloaded ~= reloaded, "Not equal.")
  end
end

local check_fail_save = function(msg, ...)
  print("check_fail_save")
  local res, err = luaamf_local.save(...)
  ensure_equals("result", res, nil)
  ensure_equals("error message", err, msg)
end

local check_fail_load = function(msg, v)
  print("check_fail_load")
  local res, err = luaamf_local.load(v)
  ensure_equals("result", res, nil)
  ensure_equals("error message", err, msg)
end

-- Basic tests

test "correct load" (function()
  assert(luaamf_local == amf)
  assert(type(luaamf_local.save) == "function")
  assert(type(luaamf_local.load) == "function")
end)

test "generated_data" (function()
  local loaded = {}

  for k, v in pairs(test_data) do
    print("     Key:", k)
    check_load_ok(v)
    print("")
  end
end)

test "basic_corrupt_data" (function()
  check_fail_load("can't load: corrupt data", "")
  check_fail_load("can't load: bad data type", "000")
  check_fail_load("can't load: bad data type", "bad data")
end)

test "basic_extra_data" (function()
  local s

  s = check_ok(nil)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(true)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(false)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(42)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(0.1)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(-1)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(math.pi)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(1/0)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok(-1/0)
  check_fail_load("can't load: extra data at end", s .. "-")

  s = check_ok("luaamf")
  check_fail_load("can't load: corrupt data, bad size", s .. "-")
end)

test "basic_type_tests" (function()
  -- This is the way to detect NaN
  check_fn_ok(function(lhs, rhs) return lhs ~= lhs and rhs ~= rhs end, 0/0)

  check_ok("")

  check_ok("Embedded\0Zero")

  check_ok(("longstring"):rep(1024000))

  check_fail_save("can't save: unsupported type detected", function() end)
end)

test "simple_array_check" (function()
  --[[
  \009
  \007 3+3+1
       \007 \111\110\101 one
  \006 \021 \102\105\114\115\116\032\104\101\114\101 first here
       \007 \116\119\111 two
  \006 \023 \115\101\099\111\110\100\032\104\101\114\101 second here
       \011 \116\104\114\101\101 three
  \006 \021 \116\104\105\114\100\032\104\101\114\101 third here
  \001
  \006 \011 \102\105\114\115\116 one
  \006 \013 \115\101\099\111\110\100 two
  \006 \011 \116\104\105\114\100 three
  ]]
  local res, err = luaamf_local.save({
    one = "first here",
    two = "second here",
    three = "third here",
    "first",
    "second",
    "third"})
  if err then error(err) else
    ensure_equals(
        "array",
        res,
        "\009\007\007\111\110\101\006\021\102\105\114\115\116\032\104\101\114"
     .. "\101\011\116\104\114\101\101\006\021\116\104\105\114\100\032\104\101"
     .. "\114\101\007\116\119\111\006\023\115\101\099\111\110\100\032\104\101"
     .. "\114\101\001\006\011\102\105\114\115\116\006\013\115\101\099\111\110"
     .. "\100\006\011\116\104\105\114\100"
      )
  end
end)

test:run()
