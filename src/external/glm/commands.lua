
local repo = "https://github.com/g-truc/glm.git"
local dir = path.getbasename(repo)
local current_rev = "682979ddd8b4b0839c36bb0796a71c095b71cbd6"

function getdir()
    return dir
end

function clone()
    if not os.isdir(getdir() .. "/.git") then
        os.execute("git clone " .. repo .. " " .. getdir())
    end
    os.chdir(dir)
    os.execute("git reset --hard")
    os.execute("git fetch origin master")
    os.execute("git checkout " .. current_rev)
end

function buildLinux()
end

function buildWindows()
end

