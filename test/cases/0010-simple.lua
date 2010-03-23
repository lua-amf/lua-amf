-- 0010-simple.lua: Lua-amf test suite
-- This file is a part of lua-amf library
-- Copyright (c) lua-amf authors (see file `COPYRIGHT` for the license)

package.cpath = "../lib/?.so;lib/?.so;"
local luaamf_local = require('luaamf')

local print, require, assert = print, require, assert

assert(pcall(dofile, 'lua-nucleo/strict.lua'), "lua-nucleo not found.")
dofile('lua-nucleo/import.lua')

local make_suite = select(1, ...)
assert(type(make_suite) == "function")
local test = make_suite("luaamf", { })

local test_data = assert(dofile('test/data/AMF_generated.lua'))

-- Utility functions

local escape_string = function(str)
  return str:gsub(
      "[^0-9A-Za-z_%- :]",
      function(c)
        return ("\\%03u"):format(c:byte())
      end
    )
end

local ensure_equals = function(msg, actual, expected)
  if actual ~= expected then
    error(
        msg..":\n  actual: `"..escape_string(tostring(actual))
        .."`\nexpected: `"..escape_string(tostring(expected)).."'"
      )
  end
end

local function deepequals(lhs, rhs)
  if type(lhs) ~= "table" or type(rhs) ~= "table" then
    return lhs == rhs
  end

  local checked_keys = {}

  for k, v in pairs(lhs) do
    checked_keys[k] = true
    if not deepequals(v, rhs[k]) then
      return false
    end
  end

  for k, v in pairs(rhs) do
    if not checked_keys[k] then
      return false -- extra key
    end
  end

  return true
end

-- Test helper functions

local check_loaded_ok = function(eq, saved, data)
  assert(eq(data, luaamf.load(saved)), "Not equal.")
  return saved
end

local check_fn_ok = function(eq, ...)
  local saved = assert(luaamf.save(...))
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
  local loaded = luaamf.load(data)
  print("  Loaded:", loaded, "(type: " .. type(loaded) .. ")")
  local saved = luaamf.save(loaded)
  -- this test wont pass due to integer <--> number <--> double decoding
  -- assert(deepequals(data, saved), "Not equal.")
  local reloaded = luaamf.load(saved)
  print("Reloaded:", reloaded, "(type: " .. type(reloaded) .. ")")
  if loaded == loaded then
    assert(deepequals(loaded, reloaded), "Not equal.")
  else
    assert(loaded ~= loaded and reloaded ~= reloaded, "Not equal.")
  end
end

local check_fail_save = function(msg, ...)
  print("check_fail_save")
  local res, err = luaamf.save(...)
  ensure_equals("result", res, nil)
  ensure_equals("error message", err, msg)
end

local check_fail_load = function(msg, v)
  print("check_fail_load")
  local res, err = luaamf.load(v)
  ensure_equals("result", res, nil)
  ensure_equals("error message", err, msg)
end

-- Basic tests

test "correct load" (function()
  assert(luaamf_local == luaamf)
  assert(type(luaamf.save) == "function")
  assert(type(luaamf.load) == "function")
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

test:run()
