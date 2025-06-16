#set terminal pngcairo size 1600,900
#set output "test.png"

set datafile separator ','

set grid

set xdata time # tells gnuplot the x axis is time data
set timefmt "%s"
set format x "%d.%m.%Y"
set xlabel 'time'
set xtics rotate by 45 right

#plot "plot.csv" using 1:3 with lines, '' using 1:4 with lines


set yrange [0:1]
set ylabel 'ratio'
set ytics 0.05

# TODO: fix trend line
#f_trendline(x) = a*x + b
#fit f_trendline(x) "plot.csv" u 1:5 via a,b

plot "plot.csv" using 1:5 w p ls -1 pt 7 title ""
	#f_trendline(x) w l lw 5 lc "forest-green" title "trend"

