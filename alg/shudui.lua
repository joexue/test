-- find the max difference of a unit and all units after the unit
array = {2, 4, 1, 16, 7, 5, 11, 9}

local diff = -88888888

for i = 1, #array do
    for j = i + 1, #array do
        local d
        d = array[i] - array[j]
        if d > diff then
            diff = d
        end
    end
end

print(diff)
