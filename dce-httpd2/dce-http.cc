// ===========================================================================
//
//   Topology
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                100 Mbps, 1 ms
//
// This experience do http
//
// ===========================================================================

#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/dce-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/trace-helper.h"
#include "ccnx/misc-tools.h"
#include <sys/stat.h>
#include <sys/types.h>

using namespace ns3;
void
CreateFiles ()
{
  FILE *fp = fopen ("files-0/index.html", "wb"); 
  int i;
  for (i = 0; i < 500000;i++)
    {
      fprintf (fp, "%d\n", i);
    }
  fclose (fp);
}
int
main (int argc, char *argv[])
{
  bool useKernel = 1;

  CommandLine cmd;
  cmd.AddValue ("kernel", "Use kernel linux IP stack.", useKernel);
  cmd.Parse (argc, argv);

  mkdir ("files-0",0744);
  //CreateFiles ();

  NodeContainer nodes, n2, n3;
  nodes.Create (3);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));

  NetDeviceContainer devices;
  

  DceManagerHelper dceManager;
//  dceManager.SetTaskManagerAttribute( "FiberManagerType", StringValue ( "UcontextFiberManager" ) );
    dceManager.SetNetworkStack ("ns3::LinuxSocketFdFactory", "Library", StringValue ("liblinux.so"));
    dceManager.Install (nodes);
    LinuxStackHelper stack;
    stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.252");
  Ipv4InterfaceContainer interfaces;
  
  n2.Add (nodes.Get (0));
  n2.Add (nodes.Get (1));
  devices = pointToPoint.Install (n2);
  interfaces = address.Assign (devices);
  address.NewNetwork ();
  n3.Add (nodes.Get (1));
  n3.Add (nodes.Get (2));
  devices = pointToPoint.Install (n3);
  interfaces = address.Assign (devices);
  address.NewNetwork ();

  // setup ip routes
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  if (useKernel)
    {
      LinuxStackHelper::PopulateRoutingTables ();
      //LinuxStackHelper::RunIp (nodes.Get (0), NanoSeconds (100), "addr list");
    }
  

  RunIp (nodes.Get(1), Seconds(0.1), "sysctl -w net.ipv4.ip_forward=1");

  DceApplicationHelper dce;
  ApplicationContainer server, client;
  

  dce.SetStackSize (1 << 20);

  // Launch the server HTTP
  dce.SetBinary ("thttpd");
  dce.ResetArguments ();
  dce.ResetEnvironment ();
  //  dce.AddArgument ("-D");
  dce.SetUid (1);
  dce.SetEuid (1);
  server = dce.Install (nodes.Get (0));
  server.Start (Seconds (1));

  dce.SetBinary ("wget");
  dce.ResetArguments ();
  dce.ResetEnvironment ();
  dce.AddArgument ("-r");
  dce.AddArgument ("http://10.1.1.1/index.html");

  server = dce.Install (nodes.Get (2));
  server.Start (Seconds (2));

  pointToPoint.EnablePcapAll ("thttpd", false);

  Simulator::Stop (Seconds (600.0));
  Simulator::Run ();

  Simulator::Destroy ();

  return 0;
}

