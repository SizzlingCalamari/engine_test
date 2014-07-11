
local repo = "https://github.com/g-truc/glm.git"
local dir = path.getbasename(repo)

function getdir()
    return dir
end

function clone()
    if not os.isdir(getdir() .. "/.git") then
        os.execute("git clone " .. repo .. " " .. getdir())
    end
end

function buildLinux()
end

function buildWindows()
end

