if is_config("mirage_split", false) then
target("mirage_engine")
else
target("mirage_base")
end
  set_kind(get_config("kind"))
  add_defines("MIRAGE_BUILD_BASE")
  add_files("**.cpp|sync/*_msvc.cpp|sync/*_posix.cpp")

  on_config(function (target)
    local lock_impl
    if target:has_cxxincludes("windows.h") then
      lock_impl = "sync/*_msvc.cpp"
    elseif target:has_cxxincludes("pthread.h") then
      lock_impl = "sync/*_posix.cpp"
    else
      raise("Can't find implementation for lock.")
    end
    target:add(
        "files",
        path.translate("$(projectdir)/libs/mirage_base/"..lock_impl))
  end)
target_end()
