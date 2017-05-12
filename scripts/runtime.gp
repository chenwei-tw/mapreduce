reset
set xlabel 'datasize'
set ylabel 'time(sec)'
set xtics 20000,800,100000
set title 'perfomance comparison'
set terminal png font " Times_New_Roman,10 "
set output 'runtime.png'
set key left

plot \
"opt.txt" using 1:2 with linespoints linewidth 2 title "opt" \
