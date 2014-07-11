
function listdirs()
    if (os.get() == "windows") then
        return io.popen('dir "'..os.getcwd()..'" /b /ad'):lines()
    else
        return io.popen('ls -d */'):lines()
    end
end

local dirs = listdirs()
for dir in dirs do
    dofile(dir .. "/commands.lua")
    local old_cwd = os.getcwd()
    os.chdir(dir)
    if _ACTION == "clone" then
        clone()
    elseif _ACTION == "build" then
        if (os.get() == "windows") then
            buildWindows()
        elseif (os.get() == "linux") then
            buildLinux()
        end
    end
    os.chdir(old_cwd)
end
