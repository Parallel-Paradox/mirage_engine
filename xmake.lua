set_project("mirage_engine")
set_version("0.1.0")
set_xmakever("2.9.9")

set_languages("cxx20")
set_warnings("all", "error")

add_rules("mode.debug", "mode.release")

if is_mode("debug") then
  add_defines("MIRAGE_BUILD_DEBUG")
end

if is_config("kind", "shared") then
  add_defines("MIRAGE_BUILD_SHARED")
end

rule("check_msvc")
  on_config(function (target)
    -- print(target:tool("cxx"))
    -- TODO add msvc checker and defines
  end)
rule_end()
add_rules("check_msvc")

option("mirage_split")
  set_description("Split the shared library into multiple targets")
  set_default(false)
  after_check(function (option)
    if option:value() == true and not is_config("kind", "shared") then
      option:set("mirage_split", false)
      cprint(
          "${bright yellow}warning:${clear} --mirage_split can only be "..
          "enabled when building a shared library, reset to false.")
    end
  end)
option_end()

add_includedirs("libs")

includes("libs/mirage_base")
includes("libs/mirage_ecs")
includes("tests")

if is_config("mirage_split", false) then
  target("mirage_engine")
    set_kind(get_config("kind"))
    add_defines("MIRAGE_BUILD_ENGINE")
    add_rules("build_mirage_base")
    add_rules("build_mirage_ecs")
  target_end()
end
