rule("build_mirage_base")
  on_config(function (target)
    target:add("defines", "MIRAGE_BUILD_BASE")
    files = {
      "auto_ptr/ref_count.cpp",
      "sync/lock.cpp"
    }
    if target:has_cxxincludes("windows.h") then
      table.insert(files, "sync/lock_impl_msvc.cpp")
    elseif target:has_cxxincludes("pthread.h") then
      table.insert(files, "sync/lock_impl_posix.cpp")
    else
      raise("Can't find implementation for lock.")
    end
    for _, file in ipairs(files) do
      target:add("files",
                 path.translate("$(projectdir)/libs/mirage_base/"..file))
    end
  end)
rule_end()

if is_config("mirage_split", true) then
  target("mirage_base")
    set_kind(get_config("kind"))
    add_rules("build_mirage_base")
  target_end()
end
