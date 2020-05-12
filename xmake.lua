-- add modes: debug and release
add_rules("mode.debug", "mode.release")

-- add target
target("filew")
    -- set kind
    set_kind("binary")
    -- add files
    add_files("src/*.cpp")
    if is_plat("windows") then 
	    add_cxflags("-std:c++latest")
	else 
	    add_cxflags("-std=c++17")
	    add_ldflags("stdc++fs")
	end 

