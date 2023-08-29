#!/bin/bash
temp_b=1
rm -rf "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno" >> /dev/null
mkdir -p "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno"
#task1
#Varying Bottleneck Datarate
for i in {1..11}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --bottleneck_datarate=$temp_b --stack_choice=3 --param=1" >> "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/bottleneck_datarate.dat"
    temp_b=$(($temp_b+30))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/bottleneck_datarate_vs_throughput.png"
set xlabel "Bottleneck Datarate"
set ylabel "Throughput(Kbps)"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/bottleneck_datarate.dat" using 1:2 ls 1  title 'TCPNewReno' with linespoints ,\
     "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/bottleneck_datarate.dat" using 1:3 ls 2  title 'TcpAdaptiveReno' with linespoints
EOFMarker
#Varying Packet Loss Rate
for i in {1..5}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --packet_loss_rate_factor=$((6-$i)) --stack_choice=3 --param=2" >> "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/packet_loss_rate.dat"
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/packet_loss_rate_vs_throughput.png"
set xlabel "Packet Loss Rate"
set ylabel "Throughput(Kbps)"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/packet_loss_rate.dat" using 1:2 ls 1  title 'TCPNewReno' with linespoints ,\
     "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/packet_loss_rate.dat" using 1:3 ls 2  title 'TcpAdaptiveReno' with linespoints
EOFMarker
#Time vs Congestion Window
./ns3 run --quiet "scratch/Offline3/1905058_1.cc --stack_choice=3 --param=3" >> "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/newreno_cwd.dat"
./ns3 run --quiet "scratch/Offline3/1905058_1.cc --stack_choice=3 --param=4" >> "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/adaptivereno_cwd.dat"
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/congestion_window_vs_time.png"
set xlabel "Time(S)"
set ylabel "Congestion Window"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/newreno_cwd.dat" using 1:2 ls 1  title 'TCPNewReno' with lines ,\
     "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/adaptivereno_cwd.dat" using 1:2 ls 2  title 'TcpAdaptiveReno' with lines
EOFMarker
#Jain's Fairness Index
#Varying Bottleneck Datarate
temp_b=1
for i in {1..11}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --bottleneck_datarate=$temp_b  --stack_choice=3 --param=5" >> "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/jain_bottleneck_datarate.dat"
    temp_b=$(($temp_b+30))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/jain_vs_bottleneck_datarate.png"
set xlabel "Bottleneck Datarate"
set ylabel "Jain's Fairness Index"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/jain_bottleneck_datarate.dat" using 1:2 ls 1 title 'Jain vs Bottleneck Datarate(TcpNewReno+TcpAdaptiveReno)'  with linespoints
EOFMarker
#Varying Packet Loss Rate
for i in {1..5}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --packet_loss_rate_factor=$((6-$i)) --stack_choice=3 --param=6" >> "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/jain_packet_loss_rate.dat"
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/jain_vs_packet_loss_rate.png"
set xlabel "Packet Loss Rate"
set ylabel "Jain's Fairness Index"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_AdaptiveReno/jain_packet_loss_rate.dat" using 1:2 ls 1 title 'Jain vs Packet Loss Rate(TcpNewReno+TcpAdaptiveReno)' with linespoints
EOFMarker
#task2