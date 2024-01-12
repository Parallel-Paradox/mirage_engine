includes("../packages/googletest.lua")
add_requires("googletest 1.14.0", {configs = {main = true}})
for _, file in ipairs(os.files("*.cpp")) do
  target("test." .. path.basename(file))
    set_kind("binary")
    set_group("test")
    set_targetdir("../build/tests")
    add_files(file)
    add_deps("mirage_engine")
    -- Why add ldflags? https://github.com/xmake-io/xmake/discussions/4332
    add_packages("googletest")
    if is_plat("windows") then
      add_ldflags("/subsystem:console")
    end
  target_end()
end
