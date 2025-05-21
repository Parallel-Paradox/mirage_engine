target("test.mirage_ecs")
  set_kind("binary")
  set_group("tests")

  add_packages("googletest")

 if is_plat("windows") then
    add_ldflags("/subsystem:console")
  end

  if is_config("mirage_split", true) then
    add_deps("mirage_ecs")
  else
    add_deps("mirage_engine")
  end
  add_files("**.cpp")
target_end()
