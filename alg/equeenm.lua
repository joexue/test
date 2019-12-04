--8 quees, my solution
N = 4

function test(s, r, c)
    for i = 1, r - 1 do
        if s[i] == c or
            s[i] + (r - i) == c or
            s[i] - (r - i) == c then
            return false;
        end
    end
    return true;
end

index = 1
function prints(s)
    io.write("\n" .. index .. "\n")
    index = index + 1
    for i = 1, N do
        for j = 1, N do
            if s[i] == j then
                io.write("X ")
            else
                io.write("- ")
            end
        end
        io.write("\n")
    end
end

function add_queen(s, r)
    if r > N then
        prints(s)
    else
        for c = 1, N do
            if test(s, r, c) then
                s[r] = c
                add_queen(s, r + 1)
            end
        end
    end
end

add_queen({}, 1)
