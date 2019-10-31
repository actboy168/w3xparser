local lm = require 'luamake'

lm.rootdir = 'src'
lm:lua_library 'w3xparser' {
   sources = "*.cpp"
}

lm:build "test" {
   "$luamake", "lua", "test/test.lua",
   deps = { "w3xparser" }
}
