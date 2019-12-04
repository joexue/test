--[[
-- 2 dollars for 1 beer
-- 2 bottles can get 1 beer
-- 4 caps can get 1 beer
-- question: how many bottles of beer we can get if we have 10 dollars
--]]
beer = 0
resource = {money = {value = 10, price=2}, bottle = {value = 0, price = 2}, cap = {value = 0, price = 4}}
round = 0
print ("Round Beer Money Bottles Caps")
print ("=============================")
--print (string.format("\27[4m%5d %4d %5d %7d %4d\27[0m", round, beer, resource.money.value, resource.bottle.value, resource.cap.value))
print (string.format("%5d %4d %5d %7d %4d", round, beer, resource.money.value, resource.bottle.value, resource.cap.value))
repeat
    for key,v in pairs(resource) do
        if v.value >= v.price then
            round = round + 1
            local x = math.floor(v.value/v.price)
            resource[key].value = math.fmod(v.value, v.price) --resource[key].value - x * v.price
            beer = beer + x
            resource.bottle.value = resource.bottle.value + x
            resource.cap.value = resource.cap.value + x
            print (string.format("%5d %4d %5d %7d %4d", round, beer, resource.money.value, resource.bottle.value, resource.cap.value))
        end
    end
    --print ("money="..resource.money.value .. " bottle=" .. resource.bottle.value .. " cap=" .. resource.cap.value)
    --print ("beer = "..beer .. ";money=" .. resource.money.value .. ";bottle = ".. resource.bottle.value .. ";cap = " .. resource.cap.value )

until resource.money.value < resource.money.price and resource.bottle.value < resource.bottle.price and resource.cap.value < resource.cap.price

--print ("beer = "..beer .. ";bottle = ".. resource.bottle.value .. ";cap = " .. resource.cap.value )
