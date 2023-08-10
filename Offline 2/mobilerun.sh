#!/bin/bash
temp_n=20
temp_f=10
temp_p=100
temp_s=5
mkdir "scratch/mobilegraphs"
#Varying Number of Nodes
for i in {1..5}
do
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=$temp_n --flow=$temp_f --packets_per_second=100 --speed=5 --param=1" >> "scratch/mobilegraphs/node_throughput.dat"
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=$temp_n --flow=$temp_f --packets_per_second=100 --speed=5 --param=2" >> "scratch/mobilegraphs/node_delivery.dat"
    temp_n=$(( $temp_n + 20 ))
    temp_f=$(( $temp_f + 10 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/nodes_vs_throughput.png"
set xlabel "Number of Nodes"
set ylabel "Throughput(Mbit/s)"
set yrange [0:10]
plot "scratch/mobilegraphs/node_throughput.dat" using 1:2 title 'NodeCount VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/nodes_vs_deliveryratio.png"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio"
set yrange [0.50:1.25]
plot "scratch/mobilegraphs/node_delivery.dat" using 1:2 title 'NodeCount VS DeliveryRatio' with linespoints
EOFMarker
#Varying Number of Flows
temp_f=10
for i in {1..5}
do
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=20 --flow=$temp_f --packets_per_second=100 --speed=5 --param=3" >> "scratch/mobilegraphs/flow_throughput.dat"
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=20 --flow=$temp_f --packets_per_second=100 --speed=5 --param=4" >> "scratch/mobilegraphs/flow_delivery.dat"
    temp_f=$(( $temp_f + 10 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/flow_vs_throughput.png"
set xlabel "Number of Flows"
set ylabel "Throughput(Mbit/s)"
set yrange [0:10]
plot "scratch/mobilegraphs/flow_throughput.dat" using 1:2 title 'FlowCount VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/flow_vs_deliveryratio.png"
set xlabel "Number of Flows"
set ylabel "Packet Delivery Ratio"
set yrange [0.50:1.25]
plot "scratch/mobilegraphs/flow_delivery.dat" using 1:2 title 'FlowCount VS DeliveryRatio' with linespoints
EOFMarker
#Varying Number of Packets Per Second
temp_p=100
for i in {1..5}
do
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=20 --flow=10 --packets_per_second=$temp_p --speed=5 --param=5" >> "scratch/mobilegraphs/packet_throughput.dat"
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=20 --flow=10 --packets_per_second=$temp_p --speed=5 --param=6" >> "scratch/mobilegraphs/packet_delivery.dat"
    temp_p=$(( $temp_p + 100 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/packets_vs_throughput.png"
set xlabel "Number of Packets per Second"
set ylabel "Throughput(Mbit/s)"
set yrange [0:10]
plot "scratch/mobilegraphs/packet_throughput.dat" using 1:2 title 'PacketperSecondCount VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/packet_vs_deliveryratio.png"
set xlabel "Number of Packets per Second"
set ylabel "Packet Delivery Ratio"
set yrange [0.50:1.25]
plot "scratch/mobilegraphs/packet_delivery.dat" using 1:2 title 'PacketperSecondCount VS DeliveryRatio' with linespoints
EOFMarker
#Varying speed Area
temp_s=5
for i in {1..5}
do
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=20 --flow=10 --packets_per_second=100 --speed=$temp_s --param=7" >> "scratch/mobilegraphs/speed_throughput.dat"
    ./ns3 run --quiet "scratch/mobilewifi.cc --nWifi=20 --flow=10 --packets_per_second=100 --speed=$temp_s --param=8" >> "scratch/mobilegraphs/speed_delivery.dat"
    temp_s=$(( $temp_s + 5 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/speed_vs_throughput.png"
set xlabel "Speed"
set ylabel "Throughput(Mbit/s)"
set yrange [0:10]
plot "scratch/mobilegraphs/speed_throughput.dat" using 1:2 title 'Speed VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/mobilegraphs/speed_vs_deliveryratio.png"
set xlabel "Speed"
set ylabel "Packet Delivery Ratio"
set yrange [0.50:1.25]
plot "scratch/mobilegraphs/speed_delivery.dat" using 1:2 title 'Speed VS DeliveryRatio' with linespoints
EOFMarker