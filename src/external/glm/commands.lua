
local repo = "https://github.com/g-truc/glm.git"
local branch = "0.9.8-align"
local dir = path.getbasename(repo)

function getdir()
    return dir
end

function clone()
    if not os.isdir(getdir() .. "/.git") then
        os.execute("git clone --depth 1 --branch " .. branch .. " " .. repo .. " " .. getdir())
    end
    local success, msg, errno = os.chdir(dir)
    if not success then
        error(msg)
    end
    os.execute("git checkout -- .")
    os.execute("git clean -ffdx .")
end

function buildLinux()
end

function buildWindows()
end

