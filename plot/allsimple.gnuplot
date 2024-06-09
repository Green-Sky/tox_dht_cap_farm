#set terminal pngcairo size 1600,900
#set output "test.png"

set datafile separator ','

set grid

set xdata time # tells gnuplot the x axis is time data
set xlabel 'time'
set xtics rotate by 45 right

#plot "plot.csv" using 1:3 with lines, '' using 1:4 with lines


set yrange [0:1]
set ylabel 'ratio'

f_trendline(x) = a*x + b
fit f_trendline(x) "plot.csv" u 0:5 via a,b

plot "plot.csv" u 0:5:xtic(strftime("%d.%m.%Y %H:%M",column(1))) w p ls -1 pt 7 title "",\
	f_trendline(x) w l lw 5 lc "forest-green" title "trend"

