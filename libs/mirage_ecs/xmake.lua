rule("build_mirage_ecs")
  on_config(function (target)
    target:add("defines", "MIRAGE_BUILD_ECS")
    target:add("files",
               path.translate("$(projectdir)/libs/mirage_ecs/**.cpp"))
  end)
rule_end()

if is_config("mirage_split", true) then
  target("mirage_ecs")
    set_kind(get_config("kind"))
    add_rules("build_mirage_ecs")
  target_end()
end
