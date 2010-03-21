-- test.lua: tests for all modules of the library
-- This file is a part of lua-nucleo library
-- Copyright (c) lua-nucleo authors (see file `COPYRIGHT` for the license)

local run_tests = assert(
    assert(
        assert(
            loadfile('lua-nucleo/suite.lua'), 
            "Lua-nucleo link not found in library directory"
          )()
      ).run_tests
  )

local tests_pr = assert(assert(loadfile('test/test-list.lua'))())

local parameters_list = {}
local n = 1
-- TODO: we need to implement input params parser and default values injector
if select(n, ...) == "--strict" then
  parameters_list.strict_mode = true
  n = 2
else
  parameters_list.strict_mode = false
end
parameters_list.seed_value = 12345


local pattern = select(n, ...) or ""
assert(type(pattern) == "string")

local test_r = {}
for _, v in ipairs(tests_pr) do
  -- Checking directly to avoid escaping special characters (like '-')
  -- when running specific test
  if v == pattern or string.match(v, pattern) then
    test_r[#test_r + 1] = "test/cases/"..v..".lua"
  end
end

if #test_r == 0 then
  error("no tests match pattern `" .. pattern .. "'")
end

if pattern ~= "" then
  print(
      "Running " .. #test_r .. " test(s) matching pattern `" .. pattern .. "'"
     )
end

run_tests(test_r, parameters_list)
