includes("../deps/googletest.lua")
-- Config "shared" can add the link search path to the target
-- https://github.com/xmake-io/xmake-repo/issues/7152
add_requires("googletest 1.17.0", {configs = {shared = true}})

includes("mirage_base")
includes("mirage_ecs")

-- -- Remove links "gtest_main" to customize main function
-- -- https://xmake.io/#/manual/project_target?id=targetadd_packages
-- add_packages("googletest", {links = {"gtest", "gmock", "gtest_main"}})

-- -- Why add ldflags? https://github.com/xmake-io/xmake/discussions/4332
-- if is_plat("windows") then
--   add_ldflags("/subsystem:console")
-- end
