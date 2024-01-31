includes("../package/googletest.lua")
add_requires("googletest 1.14.0", {configs = {main = true}})

target("test.mirage_framework")
  set_kind("binary")
  set_group("test")
  set_targetdir("$(buildir)/test")
  add_files("mirage_framework/**.cpp")
  add_deps("mirage_engine")
  -- Why add ldflags? https://github.com/xmake-io/xmake/discussions/4332
  add_packages("googletest")
  if is_plat("windows") then
    add_ldflags("/subsystem:console")
  end
target_end()
