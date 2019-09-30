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

class MyApp: public ns3::Application{
public:

	MyApp();
	virtual ~MyApp();

	void Setup(ns3::Ptr<ns3::Socket> socket, ns3::Address address, uint32_t packetSize, uint32_t nPackets,ns3:: DataRate dataRate);
	void ChangeRate(ns3::DataRate newrate);

private:
	virtual void StartApplication(void);
	virtual void StopApplication(void);

	void ScheduleTx(void);
	void SendPacket(void);

	ns3::Ptr<ns3::Socket>     m_socket;
	ns3::Address         m_peer;
	uint32_t        m_packetSize;
	uint32_t        m_nPackets;
	ns3::DataRate        m_dataRate;
	ns3::EventId         m_sendEvent;
	bool            m_running;
	uint32_t        m_packetsSent;
};

MyApp::MyApp()
	: m_socket(0),
	m_peer(),
	m_packetSize(0),
	m_nPackets(0),
	m_dataRate(0),
	m_sendEvent(),
	m_running(false),
	m_packetsSent(0) {
}

MyApp::~MyApp() {
	m_socket = 0;
}

void
MyApp::Setup(ns3::Ptr<ns3::Socket> socket, ns3::Address address, uint32_t packetSize, uint32_t nPackets, ns3::DataRate dataRate) {
	m_socket = socket;
	m_peer = address;
	m_packetSize = packetSize;
	m_nPackets = nPackets;
	m_dataRate = dataRate;
}

void
MyApp::StartApplication(void) {
	m_running = true;
	m_packetsSent = 0;
	m_socket->Bind();
	m_socket->Connect(m_peer);
	SendPacket();
}

void
MyApp::StopApplication(void) {
	m_running = false;

	if (m_sendEvent.IsRunning()) {
		ns3::Simulator::Cancel(m_sendEvent);
	}

	if (m_socket) {
		m_socket->Close();
	}
}

void
MyApp::SendPacket(void) {
	ns3::Ptr<ns3::Packet> packet = ns3::Create<ns3::Packet>(m_packetSize);
	m_socket->Send(packet);

	if (++m_packetsSent < m_nPackets) {
		ScheduleTx();
	}
}

void
MyApp::ScheduleTx(void) {
	if (m_running) {
		ns3::Time tNext(ns3::Seconds(m_packetSize * 8 / static_cast<double> ( m_dataRate.GetBitRate() )));
		m_sendEvent = ns3::Simulator::Schedule(tNext, &MyApp::SendPacket, this);
	}
}
 
