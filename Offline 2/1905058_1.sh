#!/bin/bash
temp_n=20
temp_f=10
temp_p=100
temp_c=1
rm -rf "scratch/staticgraphs" >> /dev/null
mkdir "scratch/staticgraphs"
#Varying Number of Nodes
for i in {1..5}
do
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=$temp_n --flow=$temp_f --packets_per_second=100 --coverage_area=1 --param=1" >> "scratch/staticgraphs/node_throughput.dat"
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=$temp_n --flow=$temp_f --packets_per_second=100 --coverage_area=1 --param=2" >> "scratch/staticgraphs/node_delivery.dat"
    temp_n=$(( $temp_n + 20 ))
    temp_f=$(( $temp_f + 10 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/nodes_vs_throughput.png"
set xlabel "Number of Nodes"
set ylabel "Throughput(Mbit/s)"
plot "scratch/staticgraphs/node_throughput.dat"  title 'NodeCount VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/nodes_vs_deliveryratio.png"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio"
plot "scratch/staticgraphs/node_delivery.dat"  title 'NodeCount VS DeliveryRatio' with linespoints
EOFMarker
#Varying Number of Flows
temp_f=10
for i in {1..5}
do
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=20 --flow=$temp_f --packets_per_second=100 --coverage_area=1 --param=3" >> "scratch/staticgraphs/flow_throughput.dat"
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=20 --flow=$temp_f --packets_per_second=100 --coverage_area=1 --param=4" >> "scratch/staticgraphs/flow_delivery.dat"
    temp_f=$(( $temp_f + 10 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/flow_vs_throughput.png"
set xlabel "Number of Flows"
set ylabel "Throughput(Mbit/s)"
plot "scratch/staticgraphs/flow_throughput.dat"  title 'FlowCount VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/flow_vs_deliveryratio.png"
set xlabel "Number of Flows"
set ylabel "Packet Delivery Ratio"
plot "scratch/staticgraphs/flow_delivery.dat"  title 'FlowCount VS DeliveryRatio' with linespoints
EOFMarker
#Varying Number of Packets Per Second
temp_p=100
for i in {1..5}
do
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=20 --flow=10 --packets_per_second=$temp_p --coverage_area=1 --param=5" >> "scratch/staticgraphs/packet_throughput.dat"
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=20 --flow=10 --packets_per_second=$temp_p --coverage_area=1 --param=6" >> "scratch/staticgraphs/packet_delivery.dat"
    temp_p=$(( $temp_p + 100 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/packets_vs_throughput.png"
set xlabel "Number of Packets per Second"
set ylabel "Throughput(Mbit/s)"
plot "scratch/staticgraphs/packet_throughput.dat"  title 'PacketperSecondCount VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/packet_vs_deliveryratio.png"
set xlabel "Number of Packets per Second"
set ylabel "Packet Delivery Ratio"
plot "scratch/staticgraphs/packet_delivery.dat"  title 'PacketperSecondCount VS DeliveryRatio' with linespoints
EOFMarker
#Varying Coverage Area
temp_c=1
for i in {1..5}
do
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=40 --flow=20 --packets_per_second=100 --coverage_area=$temp_c --param=7" >> "scratch/staticgraphs/coverage_throughput.dat"
    ./ns3 run --quiet "scratch/1905058_1.cc --nWifi=40 --flow=20 --packets_per_second=100 --coverage_area=$temp_c --param=8" >> "scratch/staticgraphs/coverage_delivery.dat"
    temp_c=$(( $temp_c + 1 ))
done
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/coverage_vs_throughput.png"
set xlabel "Coverage Area"
set ylabel "Throughput(Mbit/s)"
plot "scratch/staticgraphs/coverage_throughput.dat"  title 'Coverage Area VS Throughput' with linespoints
EOFMarker
gnuplot -persist <<-EOFMarker
set terminal png 
set output "scratch/staticgraphs/coverage_vs_deliveryratio.png"
set xlabel "Coverage Area"
set ylabel "Packet Delivery Ratio"
plot "scratch/staticgraphs/coverage_delivery.dat"  title 'Coverage Area VS DeliveryRatio' with linespoints
EOFMarker