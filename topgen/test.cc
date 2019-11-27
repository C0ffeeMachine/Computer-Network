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
	NodeContainer All;
	All.Create(3);
}
