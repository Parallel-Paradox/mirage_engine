set_project("mirage_engine")
set_version("0.1.0")
set_xmakever("2.8.3")

set_languages("cxx20")
set_warnings("all", "error")

add_rules("mode.debug", "mode.release")

if is_config("kind", "shared") then
  add_defines("MIRAGE_BUILD_SHARED")
end

if is_mode("debug") then
  add_defines("MIRAGE_BUILD_DEBUG")
end

target("mirage_engine")
  add_defines("MIRAGE_BUILD")
  set_kind(get_config("kind"))
  add_files("src/**.cpp")
  add_headerfiles("src/(**.hpp)")
  add_includedirs("src", {public = true})
target_end()
