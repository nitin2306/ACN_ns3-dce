#dce-iperf

##Objective:
	It sets up a point-to-point network between two nodes and runs an iperf client-server application to measure network performance.

##Topology:
	1. The network topology consists of two nodes connected by a point-to-point link. 
	2. Each node has its own IP address automatically assigned. 
	3. The iperf client runs on one node while the iperf server runs on the other. 
	4. The network is simulated with a bandwidth of 5Mbps and a delay of 2ms (Configurable).

##How to use:
    1. Ensure you have the necessary ns-3 network simulator installed.
    2. Include the required ns-3 modules and libraries as stated in the code.
    3. Compile the code with the ns-3 environment.
    4. Run the compiled program.

##output:
	The output includes the performance of the network. Basically, the latency and throughput between two nodes.
