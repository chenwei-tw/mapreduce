reset
set xlabel 'DATASIZE'
set ylabel 'time(sec)'
set xtics 20000,8000,108000
set title 'perfomance comparison'
set terminal png font " Times_New_Roman, 10"
set output 'runtime.png'
set key left

plot \
"orig.txt" using 1:2 with linespoints linewidth 2 title "orig" \
