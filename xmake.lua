set_project("mirage_engine")
set_version("0.1.0")
set_xmakever("2.8.3")

set_languages("cxx20")
set_warnings("all", "error")

add_rules("mode.debug", "mode.release")

-- mirage_engine

if is_config("kind", "shared") then
  add_defines("MIRAGE_BUILD_SHARED")
end

target("mirage_engine")
  add_defines("MIRAGE_BUILD")
  set_kind(get_config("kind"))
  add_files("src/**.cpp")
  add_includedirs("src", {public = true})
  add_headerfiles("src/(**.hpp)")
target_end()

-- examples

option("examples")
  set_default(false)
  set_description("Enable examples")
option_end()

if get_config("examples") == true then
  includes("examples")
end

-- test

option("test")
  set_default(false)
  set_description("Enable unittests")
option_end()

if get_config("test") == true then
  includes("tests")
end

task("unittest")
  on_run(function ()
    os.exec("xmake config -m release --test=yes --examples=no --kind=shared")
    os.exec("xmake build -g test")
    os.exec("xmake run -g test")
  end)
  set_menu {
    usage = "xmake test_all",
    description = "Run all test cases"
  }
task_end()
