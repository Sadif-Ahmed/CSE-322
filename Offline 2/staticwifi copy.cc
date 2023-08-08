/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/gnuplot.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   *    *    *    *
//                                  AP
//                                     Wifi 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Static High Speed Wifi");

ApplicationContainer sender_apps;
ApplicationContainer receiver_apps;

uint64_t num_sent_packet=0;
uint64_t num_received_packet=0;
double throughput;
double packet_delivery_ratio;
uint64_t num_received_bits=0;
uint64_t recent_num_received_bits=0;
double recent_time=0;

void SourceTcpTrace(Ptr<const Packet> pkt)
{
    num_sent_packet++;
}
void DestTcpTrace(Ptr<const Packet> packet_ptr,const Address &address)
{
    num_received_packet++;
    num_received_bits += packet_ptr->GetSize()*8;
}
void delivery_ratio_calculation()
{
    double time = Simulator::Now().ToDouble(Time::S);
    packet_delivery_ratio = (num_received_packet)/((num_sent_packet)*1.0);
    std::string msg = "At time " +std::to_string(time)+" Packet Delivery Ratio : "+ std::to_string(packet_delivery_ratio); 
    NS_LOG_UNCOND(msg);
    Simulator::Schedule(MilliSeconds(500), &delivery_ratio_calculation);
}
void througput_calculation()
{
    double time = Simulator::Now().ToDouble(Time::S);
    throughput = (num_received_bits-recent_num_received_bits)/(time-recent_time)/1e6 / 0.5;
    std::string msg ="At time " +std::to_string(time)+ " Throughput : " + std::to_string(throughput);
    NS_LOG_UNCOND(msg<<" Mbit/s");
    recent_num_received_bits=num_received_bits;
    recent_time=time;
    Simulator::Schedule(MilliSeconds(500), &througput_calculation);
}
void plot(std::string filename)
{
std::string graphicsFileName        = "scratch/"+filename + ".png";
std::string plotFileName            = "scratch/"+filename + ".plt";
std::string plotTitle               = "2-D Plot";
std::string dataTitle               = "2-D Data";

// Instantiate the plot and set its title.
Gnuplot plot(graphicsFileName);
plot.SetTitle(plotTitle);

// Make the graphics file, which the plot file will create when it
// is used with Gnuplot, be a PNG file.
plot.SetTerminal("png");

// Set the labels for each axis.
plot.SetLegend("X Values", "Y Values");

// Set the range for the x axis.
plot.AppendExtra("set xrange [-6:+6]");

// Instantiate the dataset, set its title, and make the points be
// plotted along with connecting lines.
Gnuplot2dDataset dataset;
dataset.SetTitle(dataTitle);
dataset.SetStyle(Gnuplot2dDataset::LINES_POINTS);
double x;
double y;

// Create the 2-D dataset.
for (x = -5.0; x <= +5.0; x += 1.0)
  {
    // Calculate the 2-D curve
    //
    //            2
    //     y  =  x   .
    //
    y = x * x;

    // Add this point.
    dataset.Add(x, y);
  }

// Add the dataset to the plot.
plot.AddDataset(dataset);

// Open the plot file.
std::ofstream plotFile(plotFileName.c_str());

// Write the plot file.
plot.GenerateOutput(plotFile);


// Close the plot file.
//plotFile.close();
}
class static_wifi
{
    public:
    uint32_t nWifi = 20;
    uint32_t flow = 10;
    uint32_t packets_per_second = 100;
    uint32_t coverage_area = 1;
    uint32_t tx_range = 5; 
    uint32_t packet_size = 1024; 
    uint64_t num_sent_packet=0;
    uint64_t num_received_packet=0;
          
    static_wifi(uint32_t nWifi,uint32_t flow,uint32_t packets_per_second,uint32_t coverage_area,uint32_t tx_range,uint32_t packet_size)
    {
        this->nWifi=nWifi;
        this->flow=flow;
        this->packets_per_second=packets_per_second;
        this->coverage_area=coverage_area;
        this->tx_range=tx_range;
        this->packet_size=packet_size;
    }
    void setup_topology_generate_flow()
    {
            /* Configure TCP Options */
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(packet_size));

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    NodeContainer sender_wifiStaNodes;
    sender_wifiStaNodes.Create((nWifi-2)/2);
    NodeContainer sender_wifiApNode = p2pNodes.Get(0);

    NodeContainer receiver_wifiStaNodes;
    receiver_wifiStaNodes.Create((nWifi-2)/2);
    NodeContainer receiver_wifiApNode = p2pNodes.Get(1);

    // Physical Layer
    // YANS model - Yet Another Network Simulator
    YansWifiChannelHelper sender_channel = YansWifiChannelHelper::Default();
    sender_channel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", ns3::DoubleValue(coverage_area * tx_range));
    YansWifiPhyHelper sender_phy;
    sender_phy.SetChannel(sender_channel.Create()); // share the same wireless medium 

    YansWifiChannelHelper receiver_channel = YansWifiChannelHelper::Default();
    receiver_channel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", ns3::DoubleValue(coverage_area * tx_range));
    YansWifiPhyHelper receiver_phy;
    receiver_phy.SetChannel(receiver_channel.Create());

    // Data Link Layer
    // SSid used to set the "ssid" Attribute in the mac layer implementation
    // The (SSID) is the network name used to logically 
    // identify the wireless network. 
    // Each network will have a single SSID that identifies the network, 
    // and this name will be used by clients to connect to the network.
    WifiMacHelper sender_mac;
    Ssid sender_ssid = Ssid("Sender-Wifi"); // creates an 802.11 service set identifier (SSID) 

    WifiMacHelper receiver_mac;
    Ssid receiver_ssid = Ssid("Receiver-Wifi"); // creates an 802.11 service set identifier (SSID) 


    WifiHelper sender_wifi;
    WifiHelper receiver_wifi;
    // ActiveProbing false -  probe requests will not be sent by MACs created by this
    // helper, and stations will listen for AP beacons.
    NetDeviceContainer sender_staDevices;
    sender_mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(sender_ssid), "ActiveProbing", BooleanValue(false));
    sender_staDevices = sender_wifi.Install(sender_phy, sender_mac, sender_wifiStaNodes);

    NetDeviceContainer receiver_staDevices;
    receiver_mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(receiver_ssid), "ActiveProbing", BooleanValue(false));
    receiver_staDevices = receiver_wifi.Install(receiver_phy, receiver_mac, receiver_wifiStaNodes);

    NetDeviceContainer sender_apDevices;
    sender_mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(sender_ssid));
    sender_apDevices = sender_wifi.Install(sender_phy, sender_mac, sender_wifiApNode);

    NetDeviceContainer receiver_apDevices;
    receiver_mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(receiver_ssid));
    receiver_apDevices = receiver_wifi.Install(receiver_phy, receiver_mac, receiver_wifiApNode);

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    
    mobility.Install(sender_wifiStaNodes);
    mobility.Install(sender_wifiApNode);

    mobility.Install(receiver_wifiStaNodes);
    mobility.Install(receiver_wifiApNode);

    // To get a mobility model and print their position
    // Ptr<MobilityModel> mob = wifiStaNodes.Get(1)->GetObject<MobilityModel>();
    // std::cout << mob->GetPosition().x << " " << mob->GetPosition().y << std::endl;

    
    

    InternetStackHelper stack;
    stack.Install(sender_wifiApNode);
    stack.Install(sender_wifiStaNodes);
    stack.Install(receiver_wifiApNode);
    stack.Install(receiver_wifiStaNodes);

    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    Ipv4InterfaceContainer sender_apInterfaces,sender_staInterfaces;
    address.SetBase("10.1.2.0", "255.255.255.0");
    sender_staInterfaces = address.Assign(sender_staDevices);
    sender_apInterfaces = address.Assign(sender_apDevices);

    Ipv4InterfaceContainer receiver_apInterfaces,receiver_staInterfaces;
    address.SetBase("10.1.3.0", "255.255.255.0");
    receiver_staInterfaces = address.Assign(receiver_staDevices);
    receiver_apInterfaces = address.Assign(receiver_apDevices);

    /* Populate routing table */
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    uint32_t i;
    for(i=1;i<=flow;i++)
    {
        OnOffHelper sender_helper("ns3::TcpSocketFactory", 
            (InetSocketAddress(
                receiver_staInterfaces.GetAddress((i)%((nWifi-2)/2)), 9)));
    sender_helper.SetAttribute("PacketSize", 
        UintegerValue(packet_size));
    sender_helper.SetAttribute("OnTime", StringValue(
        "ns3::ConstantRandomVariable[Constant=1]"));
    sender_helper.SetAttribute("OffTime", StringValue(
        "ns3::ConstantRandomVariable[Constant=0]"));
    sender_helper.SetAttribute("DataRate", DataRateValue(DataRate(packets_per_second*packet_size)));

    sender_apps.Add(sender_helper.Install(sender_wifiStaNodes.Get((i)%((nWifi-2)/2))));
    }
    
    for(i=0;i<(nWifi-2)/2;i++)
    {
        PacketSinkHelper reciever_helper("ns3::TcpSocketFactory", ns3::InetSocketAddress(ns3::Ipv4Address::GetAny(), 9));
        receiver_apps.Add(reciever_helper.Install(receiver_wifiStaNodes.Get(i)));
    }

    }
    



};
int
main(int argc, char* argv[])
{
    uint32_t nWifis[5] = {20,40,60,80,100};
    uint32_t flows[5] = {10,20,30,40,50};
    uint32_t packets_per_seconds[5] = {100,200,300,400,500};
    uint32_t coverage_areas[5] = {1,2,3,4,5};
    uint32_t tx_range = 5; 
    uint32_t packet_size = 1024;           /* Transport layer payload size in bytes. */
    double simulationTime = 10;            /* Simulation time in seconds. */

    uint32_t nWifi = 20;
    uint32_t flow = 10;
    uint32_t packets_per_second = 100;
    uint32_t coverage_area = 1;

    
    CommandLine cmd(__FILE__);
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue("flow", "Number of flows", flow);
    cmd.AddValue("packets_per_second", "Number of packets per second", packets_per_second);
    cmd.AddValue("coverage_area", "Coverage area", coverage_area);
    cmd.Parse(argc, argv);

    //LogComponentEnable("OnOffApplication", ns3::LOG_LEVEL_INFO);
    //LogComponentEnable("PacketSink", ns3::LOG_LEVEL_INFO);

    static_wifi *topology = new static_wifi(nWifi,flow,packets_per_second,coverage_area,tx_range,packet_size);
    topology->setup_topology_generate_flow();

    uint32_t i;

    for(i=0;i<sender_apps.GetN();i++)
    {
        sender_apps.Get(i)->TraceConnectWithoutContext("Tx",MakeCallback(SourceTcpTrace));
    }
    for(i=0;i<receiver_apps.GetN();i++)
    {
        receiver_apps.Get(i)->TraceConnectWithoutContext("Rx",MakeCallback(DestTcpTrace));
    }
    

    receiver_apps.Start(Seconds(0.0));
    sender_apps.Start(Seconds(1.0)); 

    Simulator::Schedule(Seconds(1.5), &delivery_ratio_calculation);
    Simulator::Schedule(Seconds(1.5), &througput_calculation);
    Simulator::Stop(Seconds(simulationTime+1));
    ns3::Simulator::Run();
    //NS_LOG_UNCOND(num_sent_packet);
    //NS_LOG_UNCOND(num_received_packet);
    //plot("test");
    double final_time = Simulator::Now().ToDouble(Time::S);
    throughput = (num_received_bits)/final_time /1e6 / 0.5;
    std::string msg ="Average Throughput : " + std::to_string(throughput);
    NS_LOG_UNCOND(msg<<"  Mbit/s");
    packet_delivery_ratio = (num_received_packet)/(num_sent_packet*1.0);
    msg = "Average Packet Delivery Ratio : "+ std::to_string(packet_delivery_ratio); 
    NS_LOG_UNCOND(msg);
    ns3::Simulator::Destroy();
    return 0;
}
