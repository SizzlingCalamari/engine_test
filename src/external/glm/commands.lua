
local repo = "https://github.com/g-truc/glm.git"
local dir = path.getbasename(repo)

function getdir()
    return dir
end

function clone()
    if not os.isdir(getdir() .. "/.git") then
        os.execute("git clone " .. repo .. " " .. getdir())
    end
    local success, msg, errno = os.chdir(dir)
    if not success then
        error(msg)
    end
    os.execute("git reset --hard")
    os.execute("git fetch origin 0.9.8-align")
    os.execute("git checkout 0.9.8-align")
end

function buildLinux()
end

function buildWindows()
end

