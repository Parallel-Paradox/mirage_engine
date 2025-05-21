package("googletest")
  set_homepage("https://github.com/google/googletest")
  set_description("Google Testing and Mocking Framework.")
  set_license("BSD-3-Clause")

  add_urls(
      "https://github.com/google/googletest/archive/refs/tags/v$(version).zip")
  add_versions(
      "1.17.0",
      "40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff")

  add_configs("gmock", {
      description = "Build the googlemock library.",
      default = true,
      type = "boolean"
  })

  if is_plat("linux") then
    add_syslinks("pthread")
  end

  add_deps("cmake >=3.16")

  on_install(function (package)
    local configs = {}
    if package:config("shared") == true then
      table.insert(configs, "-DBUILD_SHARED_LIBS=ON")
      table.insert(configs, "-DGTEST_CREATE_SHARED_LIBRARY=ON")
      table.insert(configs, "-DGTEST_LINKED_AS_SHARED_LIBRARY=ON")
    end

    if package:config("gmock") == false then
      table.insert(configs, "-DBUILD_GMOCK=OFF")
    end

    import("package.tools.cmake").install(package, configs)
  end)

  on_test(function (package)
    assert(package:has_cxxincludes("gtest/gtest.h", {
        configs = {languages = "c++17"}
    }))

    if package:config("gmock") then
      assert(package:has_cxxincludes("gmock/gmock.h", {
          configs = {languages = "c++17"}
      }))
    end
  end)
package_end()
