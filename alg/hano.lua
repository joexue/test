-- all disk elements move from p1 to p2 thoughtout p3

local function move(disk, n, p1, p2, p3)
    if n == 1 then
        print (disk[n] .. ":" .. p1 .. "->" .. p2)
    else
        move(disk, n - 1, p1, p3, p2)
        print (disk[n] .. ":" .. p1 .. "->" .. p2)
        move(disk, n - 1, p3, p2, p1)
    end
end

d = {1, 2, 3, 4}

move(d, 4, "a", "b", "c")
