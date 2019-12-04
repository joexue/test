--8 queen, real solution, no repeat of mirror, flip, rotation
N = 8

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
        table.insert(s_all, {table.unpack(s)})
        --prints(s)
    else
        for c = 1, N do
            if test(s, r, c) then
                s[r] = c
                add_queen(s, r + 1)
            end
        end
    end
end

function flip_v(s)
    sf = {}
    for i = 1, N do
        sf[i] = s[N + 1 - i]
    end
    return sf
end

function flip_h(s)
    sf = {}

    for i = 1, N do
        sf[i] = N + 1 - s[i]
    end

    return sf
end

function rotate(s)
    local sr={}
    for i = 1, N do
        sr[s[i]] = i
    end

    return sr;
end

function compare(s1, s2)
    for i = 1, N do
        if s1[i] ~= s2[i] then
            return false
        end
    end
    return true
end

function choose_solutions(sa)
    local sr = {}
    for i = 1, #sa do
        local same = false
        for j = 1, #sr do
            ---[[
            if compare(flip_h(sa[i]), sr[j]) or
               compare(flip_v(sa[i]), sr[j]) or
               compare(flip_h(flip_v(sa[i])), sr[j]) or
               compare(rotate(sa[i]), sr[j]) or
               compare(flip_v(rotate(sa[i])), sr[j]) or
               compare(flip_h(rotate(sa[i])), sr[j]) or
               compare(flip_h(flip_v(rotate(sa[i]))), sr[j]) then
                same = true
                break
            end
            --]]
        end

        if not same then
            table.insert(sr, sa[i])
        end
    end

    return sr
end

s_all = {}
add_queen({}, 1)
local s_real = choose_solutions(s_all)
print("total solution is " .. #s_all)
print("total real solution is " .. #s_real)
for i = 1, #s_real do
    prints(s_real[i])
end
