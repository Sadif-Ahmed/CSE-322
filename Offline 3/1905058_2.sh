#!/bin/bash
temp_b=1
rm -rf "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed" >> /dev/null
mkdir -p "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed"
#task1
#Varying Bottleneck Datarate
for i in {1..11}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --bottleneck_datarate=$temp_b --stack_choice=2 --param=1" >> "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/bottleneck_datarate.dat"
    temp_b=$(($temp_b+30))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/bottleneck_datarate_vs_throughput.png"
set xlabel "Bottleneck Datarate"
set ylabel "Throughput(Kbps)"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/bottleneck_datarate.dat" using 1:2 ls 1  title 'TCPNewReno' with linespoints ,\
     "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/bottleneck_datarate.dat" using 1:3 ls 2  title 'TcpHighSpeed' with linespoints
EOFMarker
#Varying Packet Loss Rate
for i in {1..5}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --packet_loss_rate_factor=$((6-$i)) --stack_choice=2 --param=2" >> "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/packet_loss_rate.dat"
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/packet_loss_rate_vs_throughput.png"
set xlabel "Packet Loss Rate"
set ylabel "Throughput(Kbps)"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/packet_loss_rate.dat" using 1:2 ls 1  title 'TCPNewReno' with linespoints ,\
     "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/packet_loss_rate.dat" using 1:3 ls 2  title 'TcpHighSpeed' with linespoints
EOFMarker
#Time vs Congestion Window
./ns3 run --quiet "scratch/Offline3/1905058_1.cc --stack_choice=2 --param=3" >> "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/newreno_cwd.dat"
./ns3 run --quiet "scratch/Offline3/1905058_1.cc --stack_choice=2 --param=4" >> "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/highspeed_cwd.dat"
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/congestion_window_vs_time.png"
set xlabel "Time(S)"
set ylabel "Congestion Window"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/newreno_cwd.dat" using 1:2 ls 1  title 'TCPNewReno' with lines ,\
     "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/highspeed_cwd.dat" using 1:2 ls 2  title 'TcpHighSpeed' with lines
EOFMarker
#Jain's Fairness Index
#Varying Bottleneck Datarate
temp_b=1
for i in {1..11}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --bottleneck_datarate=$temp_b  --stack_choice=2 --param=5" >> "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/jain_bottleneck_datarate.dat"
    temp_b=$(($temp_b+30))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/jain_vs_bottleneck_datarate.png"
set xlabel "Bottleneck Datarate"
set ylabel "Jain's Fairness Index"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/jain_bottleneck_datarate.dat" using 1:2 ls 1 title 'Jain vs Bottleneck Datarate(TcpNewReno+TcpHighSpeed)'  with linespoints
EOFMarker
#Varying Packet Loss Rate
for i in {1..5}
do
    ./ns3 run --quiet "scratch/Offline3/1905058_1.cc --packet_loss_rate_factor=$((6-$i)) --stack_choice=2 --param=6" >> "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/jain_packet_loss_rate.dat"
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/jain_vs_packet_loss_rate.png"
set xlabel "Packet Loss Rate"
set ylabel "Jain's Fairness Index"
plot "scratch/Offline3/congestiongraphs/NewReno_vs_HighSpeed/jain_packet_loss_rate.dat" using 1:2 ls 1 title 'Jain vs Packet Loss Rate(TcpNewReno+TcpHighSpeed)' with linespoints
EOFMarker
#task2