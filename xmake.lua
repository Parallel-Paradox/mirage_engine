set_project("mirage_engine")
set_version("0.1.0")
set_xmakever("2.9.9")

set_languages("cxx20")
set_warnings("all", "error")

add_rules("mode.debug", "mode.release")

rule("check_msvc")
  on_config(function (target)
    print(target:tool())
  end)
rule_end()
add_rules("check_msvc")

includes("tests")
