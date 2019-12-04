--input a string, find all subset of string
local s = io.input():read()
print(s)
 
local s1 = {}
print(#s)
for i=1, #s do
    local match = false
    for j=1, #s1 do
        if s1[j] == string.sub(s, i, i) then
            match = true
            break
        end
    end
    if not match then
        --s1[#s1 + 1] = s[i]
        table.insert(s1, string.sub(s, i, i))
        print(#s1 .. " " .. string.sub(s, i, i))
    end
end

res = {}
for i=1, #s1 do
     res[i] = {}
     res[1][i] = s1[i]
end

for i=2, #s1 do
    for j = i, #s1 do
        for k = 1, #res[i-1] do
            if not string.match(res[i-1][k], s1[j]) then
                table.insert(res[i], s1[j] .. res[i-1][k])
            end
        end
    end
end

for i=1, #res do
    for j = 1, #res[i] do
        print(res[i][j])
    end
end
