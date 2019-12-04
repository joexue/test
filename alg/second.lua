--give a array, find the second large number
array = {2, 4, 1, 16, 7, 5, 11, 9}

local first, second
first = 0
second = 0
for i=1, #array do
    if array[i] > first then
        second = first
        first = array[i]
    elseif array[i] > second then
        second = array[i]
    end
end

print(first .. " " .. second)

