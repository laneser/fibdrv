reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci runtime'
set term png enhanced font 'Verdana,10'
set output 'plot_statistic.png'
set grid
plot [0:92][0:4000] \
'plot_input_statistic' using 1:2 with linespoints linewidth 2 title "fib\\_sequence kernel",\
'' using 1:3 with linespoints linewidth 2 title "fib\\_sequence user",\
'' using 1:4 with linespoints linewidth 2 title "fib\\_fast\\_doubling kernel",\
'' using 1:5 with linespoints linewidth 2 title "fib\\_fast\\_doubling user",\
'' using 1:6 with linespoints linewidth 2 title "fib\\_fast\\_doubling\\_ctzl kernel",\
'' using 1:6 with linespoints linewidth 2 title "fib\\_fast\\_doubling\\_ctzl user"