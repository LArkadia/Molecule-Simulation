#!/usr/bin/lua
-- realtime_plot.lua
local gnuplot = io.popen("gnuplot -persist", "w")
assert(gnuplot, "No se pudo abrir Gnuplot")

gnuplot:write("set yrange [-50:50]\n")
gnuplot:write("set xrange [0:100]\n")
gnuplot:write("set title 'Primer componente de vec1 y vec2'\n")
gnuplot:write("plot '-' with lines title 'vec1', '-' with lines title 'vec2'\n")

local vec1 = {}
local vec2 = {}
local max_points = 100
local count = 0

while true do
    local line1 = io.read("*line")
    local line2 = io.read("*line")
    if not line1 or not line2 then break end

    local x1 = tonumber(line1:match("([%-%d%.]+)"))
    local x2 = tonumber(line2:match("([%-%d%.]+)"))

    if x1 and x2 then
        table.insert(vec1, x1)
        table.insert(vec2, x2)
        count = count + 1

        if #vec1 > max_points then
            table.remove(vec1, 1)
            table.remove(vec2, 1)
        end

        -- Redibujar
        gnuplot:write("plot '-' with lines title 'vec1', '-' with lines title 'vec2'\n")
        for i, v in ipairs(vec1) do
            gnuplot:write(("%d %f\n"):format(i, v))
        end
        gnuplot:write("e\n")
        for i, v in ipairs(vec2) do
            gnuplot:write(("%d %f\n"):format(i, v))
        end
        gnuplot:write("e\n")
        gnuplot:flush()
    end
end

gnuplot:close()
