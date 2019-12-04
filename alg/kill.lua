--[[
--There are x people make a loop, the man has the gun will kill the one next to
--him then hand the gun to next live man.
--
--Suppose: The gun is hold by first person.
--Question: Who will left eventually.
--]]
p = {}
PNUM = 41
for i=1, PNUM do
    p[i] = i
end

flag = 1
while #p  > 1 do
    print("\npeople left num: " .. #p)
    flag = flag + 1
    if flag > #p then
        flag = flag - #p
    end

    print("killed people: " .. p[flag])
    table.remove(p, flag)

    --Table size change, the flag may overflow, adjust it again
    if flag > #p then
        flag = flag - #p
    end

    print("gun in people: " .. p[flag])
end

--print ("left people: " .. p[1].number)
print ("\nthe last left people: " .. p[1])
