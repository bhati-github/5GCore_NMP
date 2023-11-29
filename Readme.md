# README:


## 1. Aim of this Project
-   This project demonstrate the use of newly created NMP (Network Message Protocol) 
    which can be used for information exchange between telecommunication network nodes.
-   You will see simulation of NGAP procedures, PFCP packet flows carried inside NMP packets.
-   It is a generic information exchange protocol which can be used across IP networks.

<br />
<br />
  
  
## 2. Motivation behind idea and implementation of this protocol
-  During development of S1AP protocol parser and 5GNAS protocol parser, the complexity in the parser leads me to think about a new protocol which can be parsed very easily and can be used across all call setup related interfaces.
   So, this leads to idea of Network Message Protocol (NMP) and eventually this demonstration project.

-  It is known that HTTP REST based APIs are used currently in 5G core network for inter-node communications. HTTP REST (Representational State Transfer) concept is
   to send complete state even if you wish to modify a single 2 byte element in target node. So, for just trying to communicate a 2 byte change in state, you are
   transferring entire state (in KB) towards target node. Imagine if your state grows to 10 MB in size for some reason, are you going to send your complete state
   of 10 MB (spanning across multiple fragmented packets) towards target node? This protocol serves as an alternative for such issues..

-  Use of SCTP protocol can be bypassed with this protocol between gnodeB and AMF.

<br />
<br />
    
    
    
## 3. Fundamental Idea behind NMP: 
    
 - Any piece of information to be exchanged between two entities can be 
    classified as one of the following:
    1) Fixed length 
    2) Variable length
    3) Combination of Fixed Length and Variable Length

    So, NMP carries this fundamental principal forward to make NMP messages 
    and parses accordingly. 
    
<br />
<br />

## 4. Explaination of NMP (Network Message Protocol) header

Packet Structure will look like as follows:
IP header + UDP header + NMP header + NMP items				

UDP port for NMP protocol is 1208 (just a random selection).

![nmp_header](https://github.com/bhati-github/5GCore_NMP/assets/49303596/3eff96ea-a8bf-4640-b136-3e37afde9f2c)


- **src_node_type**: This is the 16-bit node type of network node which generates
               the message. For example: enodeB, gnodeB, MME, AMF, SMF, UPF etc.

- **dst_node_type**: This is the 16-bit node type of network node which is the target of message.
               When a node receives a NMP message, it is the responsibility of that node 
               to match the dst_node_type present inside message with its node type.
	   
- **src_node_id**  : This is the 16-bit node id of network node which generates the message.
               All nodes present in a network should have some unique and random id's.

- **dst_node_id**  : This is the 16-bit node id of network node which is the target of message.
               When a node receives a NMP message, it is the responsibility of that node 
               to match the dst_node_id present inside message with its node id.

- **msg_type**     : This is the type of message that exists between any two nodes.
               For example: UE Attach Request, Create Session Request,
	             Create Session Response and UE Attach Response.

- **msg_item_count**: This is the count of information items that are carried by the NMP message.

- **msg_item_len** : This is the byte length of data that exists immediately after 20 bytes
                 of NMP header.


- **msg_identifier**: This is a randonly generated 32-bit identifier of the message. 
                Request and Response messages should have matching value of 
                message identifiers.


<br />
<br />
    

## 5. Explaination of items to be carried by NMP message

![item-description](https://github.com/bhati-github/5GCore_NMP/assets/49303596/22b25119-1322-4a4e-8b8f-79bd990f13ae)

<br />

    //////////////////////////////////////////////
    // Item id's carrying 1 byte value
    //////////////////////////////////////////////
    ITEM_ID__MSG_RESPONSE,
    ITEM_ID__UPLINK_QOS_PROFILE,
    ITEM_ID__DNLINK_QOS_PROFILE,
    ITEM_ID__PDR_ACTION,
    ITEM_ID__FAR_ACTION_FLAGS,
    ITEM_ID__FAR_DST_INTERFACE,

    //////////////////////////////////////////////
    // Item id's carrying 2 byte value
    //////////////////////////////////////////////
    ITEM_ID__PDR_RULE_ID,
    ITEM_ID__PDR_PRECEDENCE,
    ITEM_ID__PDR_FAR_ID,
    ITEM_ID__FAR_RULE_ID,

    //////////////////////////////////////////////
    // Item id's carrying 4 byte value
    //////////////////////////////////////////////
    ITEM_ID__MCC_MNC,
    ITEM_ID__TAC,
    ITEM_ID__CELL_ID,
    ITEM_ID__TMSI,
    ITEM_ID__UE_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_GTPU_TEID,

    //////////////////////////////////////////////
    // Item id's carrying 8 byte value
    //////////////////////////////////////////////
    ITEM_ID__IMSI,
    ITEM_ID__GTPU_SELF_IPV4_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV4_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE,

    //////////////////////////////////////////////
    // Item id's carrying 16 byte value
    //////////////////////////////////////////////
    ITEM_ID__UE_IDENTIFIER_SECRET,
    ITEM_ID__UE_IPV6_ADDR,

    //////////////////////////////////////////////
    // Item id's carrying more than 16 byte value
    // 2 bytes(item_id)  +
    // 2 bytes(item_len) +
    // actual item bytes
    //////////////////////////////////////////////
    ITEM_ID__GTPU_SELF_IPV6_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV6_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE,
    ITEM_ID__RANDOM_STRING,

    //////////////////////////////////////////////
    // Item id's carrying group of individual items
    // 2 bytes(group_item_id) +
    // 2 bytes(item_count)    +
    // 2 bytes(item_len)      +
    // actual item bytes
    //////////////////////////////////////////////
    ITEM_GROUP_ID__N3_PDR,
    ITEM_GROUP_ID__N6_PDR,
    ITEM_GROUP_ID__N3_FAR,
    ITEM_GROUP_ID__N6_FAR,

<br />
<br />

## 6. Demonstration in a sample network

   Important: Please do not compare the names of interfaces with the 5G core network
   actual interfaces. I used these interfaces as per my convenience. I personally call the
   combination of N1, N2 and N3 in this diagram as 'Call Triangle'.
  
![1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/ed4f7a2c-3e48-4649-8a52-88fc0a4381f7)



   The code inside this demonstration project illustrates the use of NMP protocol 
   across sample interfaces in this diagram. 

   N1 interface is used for call setup packets between gnodeB and AMF.
   N2 interface is used for datapath setup inside UPF.
   N3 interface carry data packets of UE via GTP-U packets.
   N6 interface is towards Internet.

   There is no SMF in this diagram as session management function is 
   integrated within AMF.

<br />
<br />


  ### 6.1 Sequence Diagram [ NG Setup with AMF ]
  ![ng-setup](https://github.com/bhati-github/5GCore_NMP/assets/49303596/2cbb3e06-1140-4b83-96b5-3582695f2124)

  ![ng-setup-pcap](https://github.com/bhati-github/5GCore_NMP/assets/49303596/586614ca-1358-4477-9701-ca66fe9ac9df)

<br />
  
  ### Parsed Message from gnodeB to AMF (NG Setup Request)

  ![ng-setup-1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/b13e95be-abef-45ec-93bd-5c2fe214d3ab)
  ![ng-setup-2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/ebd2dcd3-2aa7-4a78-be63-ee66fc091fe4)

<br />

  ### Parsed Message from AMF to gnodeB (NG Setup Response)

  ![ng-setup-response-1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/63badf4a-e355-4b1a-8d13-a483f85c20fa)
  ![ng-setup-response-2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/29eb922e-8e0f-46be-aea3-32e07d6dd68a)

<br />
  
 
  ### 6.2 SequenceDiagram [ Initial UE Registration Message and PDU Setup Request ] 
  
  ![sequence](https://github.com/bhati-github/5GCore_NMP/assets/49303596/197635e8-3506-483e-9a66-75c8bb8c7e4f)

  ![ue-setup](https://github.com/bhati-github/5GCore_NMP/assets/49303596/dae0a14b-21e0-4c74-9e02-febb9a92da51)

<br />
<br />

  ![1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/78cdfa3d-a212-447b-91da-f082723cb2cb)
  ![2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/6ab7a694-adcc-4ab8-bf1f-fb5c9ecda885)
  ![3](https://github.com/bhati-github/5GCore_NMP/assets/49303596/2121c876-1fec-4076-9d94-f4cb82d13c0d)
  ![4](https://github.com/bhati-github/5GCore_NMP/assets/49303596/1b25c0cd-ca3c-498b-9c58-7c9eb13b8893)
  ![5](https://github.com/bhati-github/5GCore_NMP/assets/49303596/e1b64aec-fe62-4f2d-81d8-8875abb94e86)
  ![6](https://github.com/bhati-github/5GCore_NMP/assets/49303596/35441ab8-8fea-4c23-b88c-6a48241f7e25)
  ![7](https://github.com/bhati-github/5GCore_NMP/assets/49303596/7b31a711-15f2-4729-a3ed-dd81234289f9)
  ![8](https://github.com/bhati-github/5GCore_NMP/assets/49303596/a7d49db6-5aa5-45e2-a819-bb90ed9a00d7)
  ![9](https://github.com/bhati-github/5GCore_NMP/assets/49303596/a0bd40ee-c79d-4ca9-9c97-028af6c1aab8)
  ![10](https://github.com/bhati-github/5GCore_NMP/assets/49303596/35fcec24-2c83-45e5-9c4a-072f9ad8e112)
  ![11](https://github.com/bhati-github/5GCore_NMP/assets/49303596/513335f2-77de-4729-a6f7-9179abfa09eb)
  ![12](https://github.com/bhati-github/5GCore_NMP/assets/49303596/4eee0c8f-7492-45a8-b1f1-8bae2906ccc6)
  ![13](https://github.com/bhati-github/5GCore_NMP/assets/49303596/d96fa75a-36ba-43c8-8a9e-ff6537e991a6)

<br />
<br />
  
## 7. Network Diagram for 3 different lab machines (gnodeB, amf, upf)

![network-three-machines](https://github.com/bhati-github/5GCore_NMP/assets/49303596/a13aecad-f258-4e17-85bc-d6e2009d3232)

   
    We need 3 machines to run the simulation and each machine should have these network interfaces:
    
    gnodeB machine eth0: used for login into the machine (it is connected to your lab network)
    gnodeB machine eth1: 10.10.10.1/24 and connected to AMF via N1 network
    gnodeB machine eth2: 3.3.3.6/24 and connected to UPF via N3 network
                         In this simulation, we don't need this eth2 interface. It is required only 
                         when a real upf data plane is connected for GTP-U packet transfer.
			 
    AMF machine eth0: used for login into the machine (it is connected to your lab network)
    AMF machine eth1: 10.10.10.2/24 and connected to gnodeB via N1 network
    AMF machine eth2: 20.20.20.3/24 and connected to UPF via N2 network


    UPF machine eth0: used for login into the machine (it is connected to your lab network)
    UPF machine eth1: 20.20.20.4/24 and connected to AMF via N2 network
    UPF machine eth2: 3.3.3.5/24 and connected to gnodeB via N3 network. GTP-U traffic flows on this interface. 
    UPF machine eth3: Connected to Internet world. This is known as N6 interface. (Not required for simulation)
    

   **Steps**:
   
    Open a terminal window in each of the linux machine. 
    
    - Left side terminal executes 'gnb' binary in gnb machine.
    - Middle terminal executes 'amf' binary in amf machine.
    - Right side terminal executes UPF control plane binary in upf machine. Actual UPF is not required for demonstration.
    
    (Use -debug option as an extra command line argument to see the parsed messages)
  

    1. Run AMF in middle terminal as follows:
       cd 5GCore_NMP/amf/
       make clean;make
       sudo ./amf -amfn1ip 10.10.10.2 -amfn2ip 20.20.20.3 -upfn2ip 20.20.20.4 -upfn3ip 3.3.3.5 -gnbreg 10.10.10.1 3.3.3.6
  

    2. Run UPF in right terminal as follows:
       cd 5GCore_NMP/upf/
       make clean;make
       sudo ./upf -upfn2ip 20.20.20.4 -amfn2ip 20.20.20.3

    3. At the end, run gnodeB in left side terminal. ( -c option tells how many user attach requests to be simulated )
       sudo ./gnb -gnbn1ip 10.10.10.1 -gnbn3ip 3.3.3.6 -amfn1ip 10.10.10.2 -c 10
       or
       sudo ./gnb -gnbn1ip 10.10.10.1 -gnbn3ip 3.3.3.6 -amfn1ip 10.10.10.2 -c 10 -debug  

    You can capture the call setup packets via tcpdump on any machine. 
    (NMP message UDP port is 1208)
	
    sudo tcpdump -i <interface_name> udp port 1208 -vvxx 
	

<br />
<br />  

## 8. Network Diagram for simulation inside a single virtual machine (Preferred approach)

    You need to have at least 4 interfaces inside your VM.

    If you are using virtualbox for creation and management of virtual machines, it is very easy to add 
    multiple network interfaces to your VM using virtualbox vm settings. Once these network interfaces 
    are created, you can bring them up and assign IP address. By default, you have at least one interface 
    created inside your vm (for example ens1). You have to add three more interfaces (ens2, ens3 and ens4).

    Suppose, default network interface ens1 have IP address 192.168.10.21/24 in your vm. 
    If you are adding three more interfaces to your vm, then bring them UP and assign IP address as follows:
    
    sudo ip link set ens2 up 
    sudo ip link set ens3 up
    sudo ip link set ens4 up 
    
    sudo ip addr add 192.168.10.22/24 dev ens2 
    sudo ip addr add 192.168.10.23/24 dev ens3  
    sudo ip addr add 192.168.10.24/24 dev ens4 
    
<br />

  
 ![vm-network](https://github.com/bhati-github/5GCore_NMP/assets/49303596/5b50217e-81eb-4310-a0af-0a3349cbce2f)

<br />
<br />          

    Once again, open three terminal windows from same vm. 
    -> Left side terminal executes 'gnb' binary.
    -> Middle terminal executes 'amf' binary.
    -> Right side terminal executes UPF control plane binary. 
       Actual UPF is not required for NMP demonstration.
    
    (Use -debug option as an extra command line argument to see the parsed messages)

    1. Run AMF in middle terminal as follows:
       cd 5GCore_NMP/amf/
       make clean;make
       sudo ./amf -amfn1ip 192.168.10.22 -amfn2ip 192.168.10.23 -upfn2ip 192.168.10.24 -upfn3ip 192.168.10.24 -gnbreg 192.168.10.21 192.168.10.21

       In above command, options are as follows:
       -amfn1ip 192.168.10.22 (AMF N1 interface IP address is 192.168.10.22)
       -amfn2ip 192.168.10.23 (AMF N2 interface IP address is 192.168.10.23)
       -upfn2ip 192.168.10.24 (UPF N2 interface IP address is 192.168.10.24)
       -upfn3ip 192.168.10.25 (UPF N3 interface IP address is 192.168.10.25)
       -gnbreg 192.168.10.21 192.168.10.21  (Register a gnodeB into AMF with its N1 interface and N3 interface details)
                                             You can simulate AMF instance with multiple gnodeB's also.

       For example, if you wish to simulate AMF with 3 gnodeB's. Then provide -gnbreg options as follows:
       -gnbreg 192.168.10.21 192.168.10.21 -gnbreg 192.168.10.31 192.168.10.31 -gnbreg 192.168.10.41 192.168.10.41  


    2. Run UPF in right terminal as follows:
       cd 5GCore_NMP/upf/
       make clean;make
       sudo ./upf -upfn2ip 192.168.10.24 -amfn2ip 192.168.10.23

       In above command, options are as follows:
       -upfn2ip 192.168.10.24 (UPF N2 interface IP address is 192.168.10.24)
       -amfn2ip 192.168.10.23 (AMF N2 interface IP address is 192.168.10.23)
       

    3. At the end, run gnodeB in left side terminal. ( -c option tells how many user attach requests to be simulated )
       sudo ./gnb -gnbn1ip 192.168.10.21 -gnbn3ip 192.168.10.21 -amfn1ip 192.168.10.22 -c 1
       or
       sudo ./gnb -gnbn1ip 192.168.10.21 -gnbn3ip 192.168.10.21 -amfn1ip 192.168.10.22 -c 1 -debug

       In above command, options are as follows:
       -gnbn1ip 192.168.10.21 (gnodeB N1 interface IP address is 192.168.10.21)
       -gnbn3ip 192.168.10.21 (gnodeB N3 interface IP address is 192.168.10.21)
       -amfn1ip 192.168.10.22 (AMF N1 interface IP address is 192.168.10.22)
       -c 10  (Simmulate upto 10 UE attach requests)
       -debug (Show complete NMP message parsing) 
	
    You can capture the call setup packets via tcpdump (NMP message UDP port is 1208)
    sudo tcpdump -i <interface_name> udp port 1208 -vvxx 

<br />
<br />  

## 9. Performance data 
   Time taken to successfully attach 65536 users (with PDN connectivity) into 
   core network is around 45 seconds with few prints.
   
   Without print statements, it takes about 30 seconds..

<br />
<br />


Thankyou so much for reading such a big file. Please contact me on my email id for any questions.
<br />
Abhishek Bhati
<br />
ab.bhati@gmail.com
<br />
https://www.linkedin.com/in/abhishek-bhati-2166b15a/
<br />
<br />
