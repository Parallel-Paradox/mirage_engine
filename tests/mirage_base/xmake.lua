target("test.mirage_base")
  set_kind("binary")
  set_group("tests")

  add_packages("googletest")
  -- Why add ldflags? https://github.com/xmake-io/xmake/discussions/4332
  if is_plat("windows") then
    add_ldflags("/subsystem:console")
  end
target_end()
