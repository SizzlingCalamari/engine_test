
function listdirs()
    return io.popen('dir "'..os.getcwd()..'" /b /ad'):lines()
end

local dirs = listdirs()
for dir in dirs do
    dofile(dir .. "/commands.lua")
    local old_cwd = os.getcwd()
    os.chdir(dir)
    if _ACTION == "clone" then
        clone()
    elseif _ACTION == "build" then
        build()
    end
    os.chdir(old_cwd)
end
