#include <fstream>
#include <string>
#include <sstream>
#include<vector>
#include <map>
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

NS_LOG_COMPONENT_DEFINE ("Evaluation-Phase1");

class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void ChangeRate(DataRate newrate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
 // NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << "\t" << newCwnd);
   *stream->GetStream () << Simulator::Now ().GetSeconds () << "\t" << oldCwnd << "\t" << newCwnd << std::endl;
}

static void
RxDrop (Ptr<PcapFileWrapper> file, Ptr<const Packet> p)
{
    NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
    file->Write(Simulator::Now(), p);
}
int main (int argc, char *argv[])
{
  std::string probeType = "ns3::Ipv4PacketProbe";
  std::string tracePath = "/NodeList/*/$ns3::Ipv4L3Protocol/Tx";
  std::string animFile = "dumbbell-animation.xml" ;  // Name of file for animation output


  CommandLine cmd;
  cmd.Parse (argc, argv);

//
// Explicitly create the nodes required by the topology (shown above).
//
  NS_LOG_INFO ("Create nodes.");

  NodeContainer leftnodes; // Left Nodes
  NodeContainer rightnodes; // right nodes
  NodeContainer router; // router
  NodeContainer topnodes; // topnodes

  int nLN = 10, nRN = 13, nR = 3,nTN=3;

  leftnodes.Create(nLN);
  topnodes.Create(nTN);
  rightnodes.Create(nRN);
  router.Create(nR);

//
// Build Link.
//
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("50Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));

  PointToPointHelper bt1;
  bt1.SetDeviceAttribute ("DataRate", StringValue ("20Mbps"));
  bt1.SetChannelAttribute ("Delay", StringValue ("20ms"));

  PointToPointHelper bt2;
  bt2.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  bt2.SetChannelAttribute ("Delay", StringValue ("10ms"));

//
// Build link net device container.
//
  std::map<std::string,NodeContainer> NC;
  std::map<std::string,NetDeviceContainer> NDC;
  std::ostringstream oss;

  for(int i = 0; i < nLN-1; i++)//Left nodes
  {								
    oss<<i<<"-r0";
	  NC.insert ( {oss.str(), NodeContainer(leftnodes.Get(i),router.Get(0)) } );
	  NDC.insert ( {oss.str(), p2p.Install (NC[oss.str()]) } );
	  oss.str ("");
	  oss.clear ();
  }

  for(int i = 0; i < nTN-1; i++)//Top nodes
  {								
    oss<<i<<"-r1";
	  NC.insert({oss.str(), NodeContainer(topnodes.Get(i),router.Get(1))});
	  NDC.insert({oss.str(), p2p.Install (NC[oss.str()])});
	  oss.str("");
	  oss.clear();
  }

  for(int i = 0; i < nRN-2; i++)//Right nodes
  {
  	oss<<i<<"-r"<<nR-1;
	  NC.insert({oss.str(), NodeContainer(rightnodes.Get(i),router.Get(nR-1))});
	  NDC.insert({oss.str(), p2p.Install (NC[oss.str()])});
	  oss.str("");
	  oss.clear();
  }

  for(int i = 0; i < nR-1; i++)//Routers
  {							
  	oss<<'r'<<i<<"-r"<<i+1;
	  NC.insert({oss.str(), NodeContainer(router.Get(i),router.Get(i+1))});
	  oss.str("");
	  oss.clear();
  }

  NDC.insert({"r0-r1", bt1.Install (NC["r0-r1"])});	//manually setting bottleneck atrributes to routers
  NDC.insert({"r1-r2", bt2.Install (NC["r1-r2"])});

//
// Error model for congestion window
//

  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  NDC["0-r2"].Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

//
// Install Internet Stack
//
  InternetStackHelper internetStack;
  internetStack.Install (leftnodes);
  internetStack.Install (rightnodes);
  internetStack.Install (topnodes);
  internetStack.Install (router);

//
// Assigning IP addresses.
//  
  NS_LOG_INFO ("Assign IP Addresses.");

  std::map<std::string,Ipv4InterfaceContainer> IC;
  std::ostringstream ip;

  Ipv4AddressHelper ipv4;
  
  for(int i = 0; i < nLN-1; i++)
  {
	  ip<<"10.1."<<i+1<<".0";
  	oss<<i<<"-r0";
  	ipv4.SetBase (ip.str().c_str(), "255.255.255.0");
	  IC.insert({oss.str(), ipv4.Assign (NDC[oss.str()])});
	  oss.str("");
	  oss.clear();
	  ip.str("");
	  ip.clear();
  }

  for(int i = 0; i < nTN-1; i++)
  {
    ip<<"10.3."<<i+1<<".0";
    oss<<i<<"-r1";
    ipv4.SetBase (ip.str().c_str(), "255.255.255.0");
    IC.insert({oss.str(), ipv4.Assign (NDC[oss.str()])});
    oss.str("");
    oss.clear();
    ip.str("");
    ip.clear();
  }

  for(int i = 0; i < nRN-2; i++)
  {
	  ip<<"10.2."<<i+1<<".0";
  	oss<<i<<"-r"<<nR-1;
	  ipv4.SetBase (ip.str().c_str(), "255.255.255.0");
	  IC.insert({oss.str(), ipv4.Assign (NDC[oss.str()])});
	  oss.str("");
	  oss.clear();
	  ip.str("");
	  ip.clear();
  }

  for(int i = 0; i < nR-1; i++)
  {
	  ip<<"10.4."<<i+1<<".0";
  	oss<<'r'<<i<<"-r"<<i+1;
	  ipv4.SetBase (ip.str().c_str(), "255.255.255.0");
	  IC.insert({oss.str(), ipv4.Assign (NDC[oss.str()])});
	  oss.str("");
	  oss.clear();
	  ip.str("");
	  ip.clear();
  }

//
// Turn on global static routing so we can actually be routed across the network.
//

  NS_LOG_INFO ("Enable static global routing.");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//
//Create TCP and UDP applications.
//

  NS_LOG_INFO ("Create Applications.");

//
// TCP connections.
//
  int activeCon = nTN+nLN-2;

  uint16_t sinkPortRight[activeCon];
  std::vector<Address> sinkAddRight;
  std::vector<PacketSinkHelper> packetSinkHelperRight;
  ApplicationContainer sinkRight[activeCon];
  Ptr<Socket> socketLeft[activeCon];
  Ptr<MyApp> app[activeCon];

  for(int i=0; i < activeCon; i++)
  {
    oss<<i<<"-r"<<nR-1;
    sinkPortRight[i]= 8080+i;
    sinkAddRight.push_back(Address (InetSocketAddress (IC[oss.str()].GetAddress, sinkPortRight[i])));
    packetSinkHelperRight.push_back(PacketSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPortRight[i])));
    sinkRight[i] = packetSinkHelperRight[i].Install (rightnodes.Get (i));
    sinkRight[i].Start (Seconds (0));
    sinkRight[i].Stop (Seconds (200.));

    socketLeft[i] = Socket::CreateSocket (leftnodes.Get (i), TcpSocketFactory::GetTypeId ());

    app[i]= CreateObject<MyApp> ();
    app[i]->Setup (socketLeft[i], sinkAddRight[i], 1040, 100000, DataRate ("100Mbps"));
    leftnodes.Get (i)->AddApplication (app[i]);
    app[i]->SetStartTime (Seconds (1));
    app[i]->SetStopTime (Seconds (200.));
  }

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("eval2.cwnd");
  socketLeft[0]->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange,stream));

  PcapHelper pcapHelper;
  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile ("eval2.pcap", std::ios::out, PcapHelper::DLT_PPP);
  NDC["0-r2"].Get (0)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback (&RxDrop, file));

  GnuplotHelper plotHelper;

  plotHelper.ConfigurePlot ("eval2",
                            "Packet Byte Count vs. Time",
                            "Time (Seconds)",
                            "Packet Byte Count");

  plotHelper.PlotProbe (probeType,
                        tracePath,
                        "OutputBytes",
                        "Packet Byte Count",
                        GnuplotAggregator::KEY_BELOW);

  AnimationInterface anim (animFile);
  anim.EnablePacketMetadata (); // Optional
  anim.EnableIpv4L3ProtocolCounters (Seconds (0), Seconds (200));
//
// Now, do the actual simulation.
//
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds(200.0));
  Simulator::Run ();
  std::cout << "Animation Trace file created:" << animFile.c_str ()<< std::endl;
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");   
}