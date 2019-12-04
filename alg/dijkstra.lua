--[[
Dijikstra algorithm
find the shortest path from a to b

Copyright Joe Xue 2019

Test case1
graph 1
a e 4
a c 1
c d 2
d b 3
e b 5

graph 2
a f 7
a c 14
a d 9
d c 2
d e 11
f e 15
f d 10
e b 6
c b 9

graph 3
a f 7
a c 14
a d 9
d c 2
d e 11
f e 15
f d 10
e b 6
c b 9
f a 7
c a 14
d a 9
c d 2
e d 11
e f 15
d f 10
b e 6
b c 9


test: copy the graph data into a txt file, e.g. g.txt
run it by "cat g.txt | lua Dijikstra.lua
--]]

local function getnode (graph, name)
	local node = graph[name]
	if not node then
		-- node does not exist; create a new one
		node = {name = name, adj = {}}
		graph[name] = node
	end
	return node
end

function readgraph ()
	local graph = {}

	for line in io.lines() do
		-- split line in two names
		local namefrom, nameto, weight = string.match(line, "(%S+)%s+(%S+)%s+(%S+)")
        --print(namefrom .. nameto)
		-- find corresponding nodes
		local from = getnode(graph, namefrom)
		local to = getnode(graph, nameto)
		-- adds 'to' to the adjacent set of 'from'
		from.adj[to] = tonumber(weight)
	end
	return graph
end

function findpath (curr, to, path, visited)
    --put the first node into the path path[node] = {nodes's names, weight}
	path = path or {[curr] = {path = curr.name, weight = 0}}
	visited = visited or {}

	if visited[curr] then
		return nil
	end

	visited[curr] = true

    --reach to the to node, stop search
	if curr == to then
		return nil
	end

	for node in pairs(curr.adj) do
        --if the noe is not in the set or the new path weight is less then the old one then replace it
        if not path[node] or path[node].weight > (path[curr].weight + curr.adj[node]) then
            if path[node] then visited[node] = nil end --we need to update the weight again
            --put the node into the set
            path[node] = {path=path[curr].path .. node.name, weight = path[curr].weight + curr.adj[node]}
		    findpath(node, to, path, visited)
        end
    end

    return path
end

function printpath (path, to)
    -- sp = path
    for node, sp in pairs(path) do
        print(node.name, ":", sp["path"], sp["weight"])
    end

    local sp = path[to]
    if sp then
        print("")
        print("Solution:", sp["path"], sp["weight"])
    else
        print("No Solution")
    end
end

g = readgraph()
a = getnode(g, "a")
b = getnode(g, "b")
p = findpath(a, b)
if p then printpath(p, b) end
