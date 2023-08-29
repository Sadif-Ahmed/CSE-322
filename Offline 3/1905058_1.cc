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

#include "tutorial-app.h"
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
#include "ns3/point-to-point-dumbbell.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"

// Default Network Topology
// ===========================================================================
//
//            t0----      -----h0
//  senders - t1---r0 --- r1---h1 - receivers
//
// ===========================================================================
//
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Static Topology");

 ApplicationContainer sender_apps;
 ApplicationContainer receiver_apps;

// uint64_t num_sent_packet=0;
// uint64_t num_received_packet=0;
double throughput1;
double throughput2;
double packet_delivery_ratio1;
double packet_delivery_ratio2;
uint64_t num_received_bits=0;
// uint64_t recent_num_received_bits=0;
// double recent_time=0;
uint32_t cwd_outflag=0;
double throughput_sum=0;
double throughput_square_sum=0;
double jain_index;

// void SourceTcpTrace(Ptr<const Packet> pkt)
// {
//     num_sent_packet++;
// }
// void DestTcpTrace(Ptr<const Packet> packet_ptr,const Address &address)
// {
//     num_received_packet++;
//     num_received_bits += packet_ptr->GetSize()*8;
// }
// void delivery_ratio_calculation()
// {
//     double time = Simulator::Now().ToDouble(Time::S);
//     packet_delivery_ratio = (num_received_packet)/((num_sent_packet)*1.0);
//     std::string msg = "At time " +std::to_string(time)+" Packet Delivery Ratio : "+ std::to_string(packet_delivery_ratio); 
//     NS_LOG_UNCOND(msg);
//     Simulator::Schedule(MilliSeconds(500), &delivery_ratio_calculation);
// }
// void througput_calculation()
// {
//     double time = Simulator::Now().ToDouble(Time::S);
//     throughput = (num_received_bits-recent_num_received_bits)/(time-recent_time)/1e6;
//     std::string msg ="At time " +std::to_string(time)+ " Throughput : " + std::to_string(throughput);
//     NS_LOG_UNCOND(msg<<" Mbit/s");
//     recent_num_received_bits=num_received_bits;
//     recent_time=time;
//     Simulator::Schedule(MilliSeconds(500), &througput_calculation);
// }

/**
 * Congestion window change callback
 *
 * \param oldCwnd Old congestion window.
 * \param newCwnd New congestion window.
 */
static void
CwndChangeflow1(uint32_t oldCwnd, uint32_t newCwnd)
{
    
    if(cwd_outflag==1)
    {
        //NS_LOG_UNCOND("Congestion Window Change(NewReno) :"<<Simulator::Now().GetSeconds() << "\t" << newCwnd);
        std::cout<<Simulator::Now().GetSeconds()<<"\t"<<newCwnd<<std::endl;
    }
}
static void
CwndChangeflow2(uint32_t oldCwnd, uint32_t newCwnd)
{
        if(cwd_outflag==2)
    {

        //NS_LOG_UNCOND("Congestion Window Change(Westwood) :"<<Simulator::Now().GetSeconds() << "\t" << newCwnd);
        std::cout<<Simulator::Now().GetSeconds()<<"\t"<<newCwnd<<std::endl;
    }
}


// //Tx Sent
// static void
// TcpSent(Ptr<const Packet> p)
// {
//      num_sent_packet++;
// }

class static_topology
{
    public:
    uint32_t packet_size=1024;
    uint32_t bottleneck_datarate;//in Mbps
    double packet_loss_rate;
    double bandwidth_delay_product;
    uint32_t sender_packet_count;
    uint32_t stack_choice ;
    static_topology(u_int32_t bottleneck_datarate,double packet_loss_rate,uint32_t sender_packet_count,uint32_t stack_choice)
    {
        this->bottleneck_datarate=bottleneck_datarate;
        this->packet_loss_rate=packet_loss_rate;
        this->sender_packet_count=sender_packet_count;
        this->stack_choice=stack_choice;
    } 
    void setup_topology_generate_flow()
    {
            /* Configure TCP Options */
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(packet_size));
    Config::SetDefault("ns3::TcpSocket::InitialCwnd", UintegerValue(1));
    // Config::SetDefault("ns3::TcpL4Protocol::RecoveryType",
    //                    TypeIdValue(TypeId::LookupByName("ns3::TcpClassicRecovery")));

    bandwidth_delay_product=bottleneck_datarate*1e6*0.1/packet_size/8;

    PointToPointHelper bottleneck,sender_link,receiver_link;
    bottleneck.SetDeviceAttribute("DataRate", StringValue(std::to_string(bottleneck_datarate)+"Mbps"));
    bottleneck.SetChannelAttribute("Delay", StringValue("100ms"));
    
    
    sender_link.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    sender_link.SetChannelAttribute("Delay", StringValue("1ms"));
    receiver_link.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    receiver_link.SetChannelAttribute("Delay", StringValue("1ms"));

    sender_link.SetQueue("ns3::DropTailQueue","MaxSize",StringValue(std::to_string(bandwidth_delay_product)+"p"));
    receiver_link.SetQueue("ns3::DropTailQueue","MaxSize",StringValue(std::to_string(bandwidth_delay_product)+"p"));




    PointToPointDumbbellHelper dbell = PointToPointDumbbellHelper(2,sender_link,2,receiver_link,bottleneck);

    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
    InternetStackHelper stack;
    stack.Install(dbell.GetLeft());
    stack.Install(dbell.GetRight());
    stack.Install(dbell.GetLeft(0));
    stack.Install(dbell.GetRight(0));
    if(stack_choice==1)
    {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpWestwoodPlus"));
    InternetStackHelper stack1;
    stack1.Install(dbell.GetLeft(1));
    stack1.Install(dbell.GetRight(1));
        
    }
    else if(stack_choice == 2)
    {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpHighSpeed"));
    InternetStackHelper stack1;
    stack1.Install(dbell.GetLeft(1));
    stack1.Install(dbell.GetRight(1));
    
    }
    else if(stack_choice == 3)
    {
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpAdaptiveReno"));
    InternetStackHelper stack1;
    stack1.Install(dbell.GetLeft(1));
    stack1.Install(dbell.GetRight(1));
    
    }
    Ipv4AddressHelper sender_address,bottleneck_address,receiver_address;

    sender_address.SetBase("10.1.1.0", "255.255.255.0");
    bottleneck_address.SetBase("10.2.1.0", "255.255.255.0");
    receiver_address.SetBase("10.3.1.0", "255.255.255.0");
    dbell.AssignIpv4Addresses(sender_address,receiver_address,bottleneck_address);

        /* Populate routing table */
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
    em->SetAttribute("ErrorRate", DoubleValue(packet_loss_rate));
    dbell.m_routerDevices.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    dbell.m_routerDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));
    

    


    
    // OnOffHelper sender_helper("ns3::TcpSocketFactory", (InetSocketAddress(dbell.GetRightIpv4Address(0), 9)));
    // sender_helper.SetAttribute("PacketSize", UintegerValue(packet_size));
    // sender_helper.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    // sender_helper.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    // sender_helper.SetAttribute("DataRate", DataRateValue(DataRate("1Gbps")));
    // sender_apps.Add(sender_helper.Install(dbell.GetLeft(0)));

    // OnOffHelper sender_helper2("ns3::TcpSocketFactory", (InetSocketAddress(dbell.GetRightIpv4Address(1), 9)));
    // sender_helper2.SetAttribute("PacketSize", UintegerValue(packet_size));
    // sender_helper2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    // sender_helper2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    // sender_helper2.SetAttribute("DataRate", DataRateValue(DataRate("1Gbps")));
    // sender_apps.Add(sender_helper2.Install(dbell.GetLeft(1)));
    uint32_t socket = 8080;

    Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(dbell.GetLeft(0), TcpSocketFactory::GetTypeId());
    Address sinkAddress (InetSocketAddress (dbell.GetRightIpv4Address (0), socket));
    
    Ptr<TutorialApp> app = CreateObject<TutorialApp> ();
    app->Setup (ns3TcpSocket, sinkAddress, 1024, sender_packet_count, DataRate("1Gbps"));
    dbell.GetLeft (0)->AddApplication (app);
    sender_apps.Add(app);
    ns3TcpSocket->TraceConnectWithoutContext("CongestionWindow", MakeCallback(&CwndChangeflow1));


    Ptr<Socket> ns3TcpSocket1 = Socket::CreateSocket(dbell.GetLeft(1), TcpSocketFactory::GetTypeId());
    Address sinkAddress2 (InetSocketAddress (dbell.GetRightIpv4Address (1), socket));

    Ptr<TutorialApp> app2 = CreateObject<TutorialApp> ();
    app2->Setup (ns3TcpSocket1, sinkAddress2, 1024, sender_packet_count , DataRate("1Gbps"));
    dbell.GetLeft (1)->AddApplication (app2);
    sender_apps.Add(app2);
    ns3TcpSocket1->TraceConnectWithoutContext("CongestionWindow", MakeCallback(&CwndChangeflow2));

   
    
    

  
    PacketSinkHelper reciever_helper("ns3::TcpSocketFactory", ns3::InetSocketAddress(ns3::Ipv4Address::GetAny(), socket));
    receiver_apps.Add(reciever_helper.Install(dbell.GetRight(0)));

    PacketSinkHelper reciever_helper2("ns3::TcpSocketFactory", ns3::InetSocketAddress(ns3::Ipv4Address::GetAny(), socket));
    receiver_apps.Add(reciever_helper2.Install(dbell.GetRight(1)));
   

    }
    



};
int
main(int argc, char* argv[])
{
    uint32_t bottleneck_datarate=50;//in Mbps
    double packet_loss_rate=1e-6;
    uint32_t packet_loss_rate_factor=5;
    int param=1;
    double simulation_time=300;
    uint32_t sender_packet_count=2e9*simulation_time/1024/8;
    uint32_t stack_choice=1;
    CommandLine cmd(__FILE__);

    cmd.AddValue("bottleneck_datarate", "Bottleneck Datarate", bottleneck_datarate);
    cmd.AddValue("packet_loss_rate_factor", "Packet Loss Rate Factor", packet_loss_rate_factor);
    cmd.AddValue("param", "Parametre", param);
    cmd.AddValue("stack_choice", "Choice of Stack", stack_choice);
    cmd.Parse(argc, argv);
    
    //LogComponentEnable("OnOffApplication", ns3::LOG_LEVEL_INFO);
    //LogComponentEnable("PacketSink", ns3::LOG_LEVEL_INFO);


    if(packet_loss_rate_factor==1)
    {
        packet_loss_rate=1e-2;
    }
    else if(packet_loss_rate_factor==2)
    {
        packet_loss_rate=1e-3;
    }
    else if(packet_loss_rate_factor==3)
    {
        packet_loss_rate=1e-4;
    }
    else if(packet_loss_rate_factor==4)
    {
        packet_loss_rate=1e-5;
    }
    else if(packet_loss_rate_factor==5)
    {
        packet_loss_rate=1e-6;
    }

    if(param == 3 )
    {
        cwd_outflag=1;  
    }
    else if(param == 4 )
    {
        cwd_outflag=2;   
    }
    NS_LOG_UNCOND("Simulation Parametres:");
    NS_LOG_UNCOND("Bottleneck Datarate: "<<bottleneck_datarate);
    NS_LOG_UNCOND("Packet Loss Rate: " << packet_loss_rate);
    NS_LOG_UNCOND("Stack Choice: " << stack_choice);

    static_topology *topology = new static_topology(bottleneck_datarate,packet_loss_rate,sender_packet_count,stack_choice);
    topology->setup_topology_generate_flow();
    

    

    // for(i=0;i<sender_apps.GetN();i++)
    // {
    //     sender_apps.Get(i)->TraceConnectWithoutContext("Tx",MakeCallback(SourceTcpTrace));
    // }
    // for(i=0;i<receiver_apps.GetN();i++)
    // {
    //     receiver_apps.Get(i)->TraceConnectWithoutContext("Rx",MakeCallback(DestTcpTrace));
        
    // }
    

    sender_apps.Start(Seconds(0.));
    sender_apps.Stop(Seconds(simulation_time-5));

    receiver_apps.Start(Seconds(0.));
    receiver_apps.Stop(Seconds(simulation_time-5));
    //Simulator::Schedule(Seconds(1.5), &delivery_ratio_calculation);
    //Simulator::Schedule(Seconds(1.5), &througput_calculation);
    Simulator::Stop(Seconds(simulation_time));

    FlowMonitorHelper flowmonitorhelper;
    Ptr<FlowMonitor> monitor = flowmonitorhelper.InstallAll();

    ns3::Simulator::Run();

    monitor->CheckForLostPackets();
    FlowMonitor::FlowStatsContainer flow_stats = monitor->GetFlowStats();
    

    double total_received_packets1=0;
    double total_received_bits1=0;
    double total_sent_packets1=0;
    double total_received_packets2=0;
    double total_received_bits2=0;
    double total_sent_packets2=0;
    double temp_throughput;
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonitorhelper.GetClassifier());
    for(FlowMonitor::FlowStatsContainer::const_iterator it=flow_stats.begin();it!=flow_stats.end();it++)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (it->first); 
            // classifier returns FiveTuple in correspondance to a flowID

      NS_LOG_UNCOND("----Flow ID:" <<it->first);
      NS_LOG_UNCOND("Src Addr:   " <<t.sourceAddress << " -- Dst Addr:   "<< t.destinationAddress);
      NS_LOG_UNCOND("Sent Packets = " <<it->second.txPackets);
      NS_LOG_UNCOND("Received Packets = " <<it->second.rxPackets);
        if(it->first%2==1)
        {
        total_sent_packets1 += it->second.txPackets;
        total_received_packets1 += it->second.rxPackets;
        total_received_bits1 += it->second.rxBytes*8;
        }
        else
        {
        total_sent_packets2 += it->second.txPackets;
        total_received_packets2 += it->second.rxPackets;
        total_received_bits2 += it->second.rxBytes*8;
        }
        temp_throughput = it->second.rxBytes*8/simulation_time /1e3;
        throughput_sum +=  temp_throughput;
        throughput_square_sum += temp_throughput*temp_throughput;        
    }

    // NS_LOG_UNCOND("Trace Calculation: ");
    // double final_time = Simulator::Now().ToDouble(Time::S);
    // throughput = (num_received_bits)/final_time /1e6 ;
    // std::string msg ="Average Throughput : " + std::to_string(throughput);
    // NS_LOG_UNCOND(msg<<"  Mbit/s");
    // packet_delivery_ratio = (num_received_packet)/(num_sent_packet*1.0);
    // NS_LOG_UNCOND("Sent Packets: "<<num_sent_packet);
    // NS_LOG_UNCOND("Received Packets: "<<num_received_packet);
    // msg = "Average Packet Delivery Ratio : "+ std::to_string(packet_delivery_ratio); 
    // NS_LOG_UNCOND(msg);



    NS_LOG_UNCOND("Flow Calculation: ");
    NS_LOG_UNCOND("Flow1: ");
    throughput1 = (total_received_bits1)/simulation_time /1e3 ;
    std::string msg ="Average Throughput : " + std::to_string(throughput1);
    NS_LOG_UNCOND(msg<<"  Kbit/s");
    packet_delivery_ratio1 = (total_received_packets1)/(total_sent_packets1*1.0);
    NS_LOG_UNCOND("Sent Packets: "<<total_sent_packets1);
    NS_LOG_UNCOND("Received Packets: "<<total_received_packets1);
    msg = "Average Packet Delivery Ratio : "+ std::to_string(packet_delivery_ratio1); 
    NS_LOG_UNCOND(msg);

    NS_LOG_UNCOND("Flow2: ");
    throughput2 = (total_received_bits2)/simulation_time /1e3 ;
    msg ="Average Throughput : " + std::to_string(throughput2);
    NS_LOG_UNCOND(msg<<"  Kbit/s");
    packet_delivery_ratio2 = (total_received_packets2)/(total_sent_packets2*1.0);
    NS_LOG_UNCOND("Sent Packets: "<<total_sent_packets2);
    NS_LOG_UNCOND("Received Packets: "<<total_received_packets2);
    msg = "Average Packet Delivery Ratio : "+ std::to_string(packet_delivery_ratio2); 
    NS_LOG_UNCOND(msg);

    jain_index=throughput_sum*throughput_sum/throughput_square_sum/flow_stats.size();
    msg = "Jain's Fairness Index : "+ std::to_string(jain_index); 
    NS_LOG_UNCOND(msg);

    if(param == 1)
    {
        std::cout << bottleneck_datarate << "\t" << throughput1 << "\t" << throughput2 << std::endl;
    }
    else if(param == 2)
    {
        std::cout << packet_loss_rate << "\t" << throughput1 <<"\t"<< throughput2 << std::endl;
    }
    else if(param == 5)
    {
        std::cout << bottleneck_datarate << "\t" << jain_index  << std::endl;
    }
    else if(param == 6)
    {
        std::cout << packet_loss_rate << "\t" << jain_index  << std::endl;
    }
   
    

    ns3::Simulator::Destroy();
    return 0;
}
