if is_config("mirage_split", false) then
target("mirage_engine")
else
target("mirage_ecs")
  add_deps("mirage_base")
end
  set_kind(get_config("kind"))
  add_defines("MIRAGE_BUILD_ECS")
  add_files("**.cpp")
target_end()
