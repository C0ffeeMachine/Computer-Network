#include "ns3/constant-position-mobility-model.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/stats-module.h"
#include "ns3/global-route-manager.h"
#include "ns3/bridge-module.h"
#include "ns3/trace-helper.h"
#include "ns3/netanim-module.h"
using namespace ns3;
#include"MyApp.h"
int main(int argc, char *argv[]) {
	std::string animFile = "test.xml";  // Name of file for animation output
	CommandLine cmd;
	cmd.Parse(argc, argv);
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate", StringValue("50Mbps"));
	p2p.SetChannelAttribute("Delay", StringValue("10ms"));
	int nnodes=11;
	NodeContainer All;
	All.Create(nnodes);

	NodeContainer nc3_0 = NodeContainer(All.Get(3),All.Get(0));
	NetDeviceContainer ndc3_0 = p2p.Install(nc3_0);

	NodeContainer nc3_1 = NodeContainer(All.Get(3),All.Get(1));
	NetDeviceContainer ndc3_1 = p2p.Install(nc3_1);

	NodeContainer nc3_2 = NodeContainer(All.Get(3),All.Get(2));
	NetDeviceContainer ndc3_2 = p2p.Install(nc3_2);

	NodeContainer nc3_4 = NodeContainer(All.Get(3),All.Get(4));
	NetDeviceContainer ndc3_4 = p2p.Install(nc3_4);

	NodeContainer nc4_9 = NodeContainer(All.Get(4),All.Get(9));
	NetDeviceContainer ndc4_9 = p2p.Install(nc4_9);

	NodeContainer nc4_5 = NodeContainer(All.Get(4),All.Get(5));
	NetDeviceContainer ndc4_5 = p2p.Install(nc4_5);

	NodeContainer nc5_6 = NodeContainer(All.Get(5),All.Get(6));
	NetDeviceContainer ndc5_6 = p2p.Install(nc5_6);

	NodeContainer nc5_7 = NodeContainer(All.Get(5),All.Get(7));
	NetDeviceContainer ndc5_7 = p2p.Install(nc5_7);

	NodeContainer nc5_8 = NodeContainer(All.Get(5),All.Get(8));
	NetDeviceContainer ndc5_8 = p2p.Install(nc5_8);

	NodeContainer nc5_10 = NodeContainer(All.Get(5),All.Get(10));
	NetDeviceContainer ndc5_10 = p2p.Install(nc5_10);

	Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
	em->SetAttribute("ErrorRate", DoubleValue(0.00001));
	ndc3_0.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(em));
	InternetStackHelper internetStack;
	internetStack.Install(All);
	Ipv4AddressHelper address;

	address.SetBase(10.1.1.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc3_0 = ipv4.Assign(ndc3_0);

	address.SetBase(10.1.2.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc3_1 = ipv4.Assign(ndc3_1);

	address.SetBase(10.1.3.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc3_2 = ipv4.Assign(ndc3_2);

	address.SetBase(10.1.4.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc3_4 = ipv4.Assign(ndc3_4);

	address.SetBase(10.1.5.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc4_9 = ipv4.Assign(ndc4_9);

	address.SetBase(10.1.6.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc4_5 = ipv4.Assign(ndc4_5);

	address.SetBase(10.1.7.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc5_6 = ipv4.Assign(ndc5_6);

	address.SetBase(10.1.8.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc5_7 = ipv4.Assign(ndc5_7);

	address.SetBase(10.1.9.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc5_8 = ipv4.Assign(ndc5_8);

	address.SetBase(10.1.10.0, "255.255.255.0");
	Ipv4InterfaceContainer ifc5_10 = ipv4.Assign(ndc5_10);

	uint16_t sinkPort6 = 8080;
	Address sinkAddr6 = InetSocketAddress(ifc5_6.GetAddress(1), sinkPort6);
	PacketSinkHelper packetSinkHelper6 = "ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort6);
	ApplicationContainer sink6 = packetSinkHelper6.Install(All.Get(6));
	sink6.Start(Seconds(0));
	sink6.Stop(Seconds(200.));

	Ptr<Socket> socket0 = Socket::CreateSocket(All.Get(0), TcpSocketFactory::GetTypeId());

	Ptr<MyApp> app0 = CreateObject<MyApp>();
	app0->Setup(socket0, sinkAddr6, 1040, 100000, DataRate("100Mbps"));
	All.Get(0)->AddApplication(app0);
	app0->SetStartTime(Seconds(1));
	app0->SetStopTime(Seconds(200.));

	uint16_t sinkPort7 = 8081;
	Address sinkAddr7 = InetSocketAddress(ifc5_7.GetAddress(1), sinkPort7);
	PacketSinkHelper packetSinkHelper7 = "ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort7);
	ApplicationContainer sink7 = packetSinkHelper7.Install(All.Get(7));
	sink7.Start(Seconds(0));
	sink7.Stop(Seconds(200.));

	Ptr<Socket> socket1 = Socket::CreateSocket(All.Get(1), TcpSocketFactory::GetTypeId());

	Ptr<MyApp> app1 = CreateObject<MyApp>();
	app1->Setup(socket1, sinkAddr7, 1040, 100000, DataRate("100Mbps"));
	All.Get(1)->AddApplication(app1);
	app1->SetStartTime(Seconds(1));
	app1->SetStopTime(Seconds(200.));

	uint16_t sinkPort8 = 8082;
	Address sinkAddr8 = InetSocketAddress(ifc5_8.GetAddress(1), sinkPort8);
	PacketSinkHelper packetSinkHelper8 = "ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort8);
	ApplicationContainer sink8 = packetSinkHelper8.Install(All.Get(8));
	sink8.Start(Seconds(0));
	sink8.Stop(Seconds(200.));

	Ptr<Socket> socket2 = Socket::CreateSocket(All.Get(2), TcpSocketFactory::GetTypeId());

	Ptr<MyApp> app2 = CreateObject<MyApp>();
	app2->Setup(socket2, sinkAddr8, 1040, 100000, DataRate("100Mbps"));
	All.Get(2)->AddApplication(app2);
	app2->SetStartTime(Seconds(1));
	app2->SetStopTime(Seconds(200.));

	uint16_t sinkPort10 = 8083;
	Address sinkAddr10 = InetSocketAddress(ifc5_10.GetAddress(1), sinkPort10);
	PacketSinkHelper packetSinkHelper10 = "ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort10);
	ApplicationContainer sink10 = packetSinkHelper10.Install(All.Get(10));
	sink10.Start(Seconds(0));
	sink10.Stop(Seconds(200.));

	Ptr<Socket> socket9 = Socket::CreateSocket(All.Get(9), TcpSocketFactory::GetTypeId());

	Ptr<MyApp> app3 = CreateObject<MyApp>();
	app3->Setup(socket9, sinkAddr10, 1040, 100000, DataRate("100Mbps"));
	All.Get(9)->AddApplication(app3);
	app3->SetStartTime(Seconds(1));
	app3->SetStopTime(Seconds(200.));

PcapHelper pcapHelper;
p2p.EnablePcapAll("eval");
AnimationInterface anim(animFile);
anim.EnablePacketMetadata(); // Optional
anim.EnableIpv4L3ProtocolCounters(Seconds(0), Seconds(200));
Simulator::Stop(Seconds(200.0));
Simulator::Run();
std::cout << "Animation Trace file created:" << animFile.c_str() << std::endl;
Simulator::Destroy();
}
}
