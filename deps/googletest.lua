package("googletest")
  set_homepage("https://github.com/google/googletest")
  set_description("Google Testing and Mocking Framework.")

  add_urls(
      "https://github.com/google/googletest/archive/refs/tags/v$(version).zip")
  add_versions(
      "1.17.0",
      "40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff")
  add_versions(
      "1.14.0",
      "1f357c27ca988c3f7c6b4bf68a9395005ac6761f034046e9dde0896e3aba00e4")

  add_configs("main", {
      description = "Link to the gtest_main entry point.",
      default = false,
      type = "boolean"
  })
  add_configs("gmock", {
      description = "Link to the googlemock library.",
      default = false,
      type = "boolean"
  })

  if is_plat("linux") then
    add_syslinks("pthread")
  end

  on_install(function (package)
    io.writefile("xmake.lua", [[
      set_languages("cxx17")

      target("gtest")
        set_kind("shared")
        set_languages("cxx17")
        add_files("googletest/src/gtest-all.cc")
        add_includedirs("googletest")
        add_includedirs("googletest/include", {public = true})
        add_headerfiles("googletest/include/(**.h)")

      target("gtest_main")
        set_kind("static")
        set_languages("cxx17")
        set_default(]] .. tostring(package:config("main")) .. [[)
        add_files("googletest/src/gtest_main.cc")
        add_includedirs("googletest")
        add_includedirs("googletest/include", {public = true})
        add_headerfiles("googletest/include/(**.h)")

      target("gmock")
        set_kind("shared")
        set_languages("cxx17")
        set_default(]] .. tostring(package:config("gmock")) .. [[)
        add_files("googlemock/src/gmock-all.cc")
        add_includedirs("googlemock", "googletest/include", "googletest")
        add_includedirs("googlemock/include", {public = true})
        add_headerfiles("googlemock/include/(**.h)")
    ]])
    import("package.tools.xmake").install(package)
  end)

  on_test(function (package)
    assert(package:has_cxxincludes("gtest/gtest.h"))

    if package:config("gmock") then
      assert(package:has_cxxincludes("gmock/gmock.h"))
    end
  end)
package_end()
