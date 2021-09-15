-------------------------------------------------------------------------------
README:
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
1. Aim of this Project
-------------------------------------------------------------------------------
    This project demonstrate the use of newly created NMP (Network Message Protocol) 
    which can be used for information exchange between telecommunication network nodes.
    In general, this protocol is not restricted to use only in telecommunication
    network node interfaces. Instead, it is a generic information exchange protocol 
    which can be used across IP networks.

-------------------------------------------------------------------------------
2. Motivation behind idea and implementation of this protocol
-------------------------------------------------------------------------------
    During development of S1AP protocol parser and 5GNAS protocol parser, the 
    complexity in the parser leads me to think about a new protocol which can 
    be parsed very easily and can be used across all call setup related interfaces.
    So, this leads to idea of Network Message Protocol (NMP) and eventually 
    this demonstration project.

    
    Fundamental Idea behind NMP: 
    
    Any piece of information to be exchanged between two entities can be 
    classified as one of the following:
    1) Fixed length 
    2) Variable length
    3) Combination of Fixed Length and Variable Length

    So, NMP carries this fundamental principal forward to make NMP messages 
    and parses accordingly. 
    
-------------------------------------------------------------------------------
3. Brief overview of protocols in 4G/5G telecommunication networks.
-------------------------------------------------------------------------------
    3.1 Existing Protocols in 4G network

    There are different kind of interfaces between nodes like enodeB, MME, SGW 
    and PGW etc.
   
    For example: 
   
    S1-MME interface exists between enodeB and MME and information exchange
    happens via S1AP protocol. S1AP packets are carried by SCTP protocol.
   
    S11 interface exists between MME and SGW. GTP-C protocol carry information
    elements for datapath setup inside SGW.

    3.2 Existing Protocols in 5G network

    There are different kind of interfaces between nodes like gnodeB, AMF, SMF 
    and UPF.

    For example:

    N1 interface exists between UE(User Equipment) and AMF. 5GNAS protocol carry 
    information over SCTP protocol on this interface.

    N2 interface between gnodeB and AMF. Once again 5GNAS protocol carry information 
    betweem these two node.

    N11 interface exists between AMF and SMF and HTTP2 protocol carry information 
    between these two nodes.

    N4 interface exists between SMF and UPF and PFCP protocol is used for datapath
    setup inside UPF.


-------------------------------------------------------------------------------	
4. Explaination of NMP (Network Message Protocol) header
-------------------------------------------------------------------------------
Packet Structure will look like as follows:
IP header + UDP header + NMP header + NMP items				

UDP port for NMP protocol is 1208 (just a random selection).


// 20 bytes NMP header
struct nmp_hdr {
    uint16_t  src_node_type;
    uint16_t  dst_node_type;
    uint16_t  src_node_id;
    uint16_t  dst_node_id;
    uint16_t  msg_type;
    uint16_t  msg_item_count;
    uint16_t  msg_item_len;
    uint16_t  rsvd;
    uint32_t  msg_identifier;
} __attribute__((packed));

src_node_type: This is the 16-bit node type of network node which generates 
               the message. For example: enodeB, gnodeB, MME, AMF, SMF, UPF etc.

dst_node_type: This is the 16-bit node type of network node which is the target of message.
               When a node receives a NMP message, it is the responsibility of that node 
               to match the dst_node_type present inside message with its node type.
	   
src_node_id  : This is the 16-bit node id of network node which generates the message.
               All nodes present in a network should have some unique and random id's.

dst_node_id  : This is the 16-bit node id of network node which is the target of message.
               When a node receives a NMP message, it is the responsibility of that node 
               to match the dst_node_id present inside message with its node id.

msg_type     : This is the type of message that exists between any two nodes.
               For example: UE Attach Request, Create Session Request,
	       Create Session Response and UE Attach Response.

msg_item_count: This is the count of information items that are carried by the NMP message.

msg_item_len : This is the byte length of data that exists immediately after 20 bytes
               of NMP header.


msg_identifier: This is a randonly generated 32-bit identifier of the message. 
                Request and Response messages should have matching value of 
                message identifiers.
				
				
-------------------------------------------------------------------------------
5. Explaination of items to be carried by NMP message
-------------------------------------------------------------------------------

Type 1 Item:  These are the items which carry fixed number of bytes as item value between two nodes.
              2 bytes of item_id + fixed amount of value (1 byte, 2 bytes, 4 bytes, 8 bytes or 16 bytes)

Type 2 Item:  These are the items which carry a variable length of item value bewteen two nodes.
              2 bytes of item_id + 2 bytes of item_len + actual bytes of item_value
			  
Item Group:   This is a group of items (item_type_1 and item_type_2)
              2 bytes of item_id + 2 bytes of item_count + 2 bytes of item_len + 
			  actual bytes of item_value (combination of item_1 and item_2)


enum item_id_t {
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
};


-------------------------------------------------------------------------------
6. NMP Items
-------------------------------------------------------------------------------

    Example of a message which carry these items between gnodeB and AMF.
    1) IMSI, 2) UE Secret and 3) 40 byte random string.
   
    IMSI is a type1 item because it is an 8 byte fixed value.
    item_id    = ITEM_ID__IMSI
    item_value = 8 bytes of user imsi  


    UE Secret is also a type1 item because secret length is always 16 bytes.
    item_id    = ITEM_ID__UE_IDENTIFIER_SECRET
    item_value = 16 bytes of secret
   
   
    40 byte random string will be carried by Type2 item
    item_id    = ITEM_ID__RANDOM_STRING
    item_len   = 40
    item_value = actual 40 bytes
   
   
    You can notice that Type1 items do not have 2 bytes of item_len after item_id.
    This is because of the reason that we can derive item_length from its item_id in case of type1 items.


  
------------------------------------------------------------------------------- 
7. Demonstration in a sample network
-------------------------------------------------------------------------------

    Please do not compare the names of interfaces with the 5G core network 
    actual interfaces. I used these interfaces as per my convenience. 
    I personally call the combination of N1, N2 and N3 in this diagram as 'Call Triangle'. 

 
                   AMF 
                 /    \
                /      \
            N1 /        \ N2
              /          \
             /            \
UE ------ gnodeB --------- UPF -------- Internet 
    Air             N3            N6


   The code inside this demonstration project illustrates the use of NMP protocol 
   across sample interfaces in this diagram. 

   N1 interface is used for call setup packets between gnodeB and AMF.
   N2 interface is used for datapath setup inside UPF.
   N3 interface carry data packets of UE via GTP-U packets.
   N6 interface is towards Internet.

   There is no SMF in this diagram as session management function is 
   integrated within AMF.


-------------------------------------------------------------------------------
8. Steps to establish a session inside UPF.
-------------------------------------------------------------------------------

   1.  gnodeB ---------> AMF                   (Msg Type = UE Attach Request)
   2.                    AMF ------------> UPF (Msg Type = Create Session Request)
   3.                    AMF <------------ UPF (Msg Type = Create Session Response)
   4.  gnodeB <--------- AMF                   (Msg Type = UE Attach Response)
   

  UE Attach Request carry two basic elements. (IMSI and UE Secret)

  AMF allocates User IP address, uplink TEID and downlink TEID for the user packets.

  AMF sends Create Session Request to UPF. Create Session Request carries N3 PDR, N6 PDR,
  N3 FAR, N6 FAR, UE IPv4 address, user IMSI.
  
  UPF sends back Create Session Response back to AMF

  AMF sends UE Attach Reponse back towards gnodeB. This packet carries uplink and downlink 
  TEID of user session. gnodeB should be aware of GTP-U parameters to process user data packets. 
   
  In this sample network, user ip address allocation, uplink and downlink TEID value generation 
  are done by a single controlling entity (AMF). gnodeB and UPF are relieved from this duty.
  This made the job of UE handover from one gnodeB to another gnodeB much easier.
  In a handover operation, bearer setup request is sent to new gnodeB and only downlink
  tunnel endpoint address changes in the handover operation. 
  Remaining parameters like uplink TEID, downlink TEID will be same.
  Moreover, once a user attaches to a UPF, session will be always present inside 
  that UPF till the lifetime of that user session. UPF will not change as long as 
  user is roaming around in a specific PLMN area which has a set of AMF, UPF and multiple gnodeB's.
  
  

-------------------------------------------------------------------------------
9. How to make binary images ?
-------------------------------------------------------------------------------

   Makefiles are really simple and the only thing to be done here is as follows:
   
   cd gnb
   make clean;make
   
   cd amf
   make clean;make
   
   cd upf
   make clean;make
   

-------------------------------------------------------------------------------   
10. Network Diagram
-------------------------------------------------------------------------------
   
           amf_n1(10.10.10.2)     -------  amf_n2(10.10.10.3)
        --------------------------| AMF |---------------------------------
        |   N1                    -------                 N2             |
        |                                                                |
        |                                                                |
        | gnb_n1                                                         |upf_n2
        |(10.10.10.1)                                                    |(10.10.10.4)
     --------                                                         ---------
-----|gnodeB|                                                         | UPF   |---------- Internet
 Air --------                                                         ---------
        |                            N3                                  |  
        |----------------------------------------------------------------|
         gnb_n3 (3.3.3.2)                                 upf_n3 (3.3.3.1)     
                                                                
   
    We need 3 machines to run the simulation and each machine should have these network interfaces:
    gnodeB machine eth0: used for login into the machine (it is connected to your lab network)
    gnodeB machine eth1: used to connect to AMF (10.10.10.1/24)
    gnodeB machine eth2: used to connect to UPF data plane (3.3.3.2/24)
                         but this simulation don't need this eth2 interface, 
			 only required when a real upf data plane is connected)
			 
    AMF machine eth0: used for login into the machine (it is connected to your lab network)
    AMF machine eth1: used to connect to gnodeB machine (10.10.10.2/24)
    AMF machine eth2: used to connect to UPF control plane (10.10.10.3/24)
    You can use a single interface eth2 to assign two IP 
    addresses (10.10.10.2/24 and 10.10.10.3/24) on it. Its up to you.
    
    UPF machine eth0: used for login into the machine (it is connected to your lab network)
    UPF machine eth1: used to connect to AMF machine (10.10.10.4/24)
    UPF machine eth2: used to connect to gnodeB N3 interface (3.3.3.1/24). 
                      Actual user data comes on this interface in real world.
    UPF machine eth3: connect to Internet world. This is known as N6 interface. (Not required for simulation)
    
     
    
    As of now, only call setup interfaces are required for simulation of call setup.

    You can see that N1 and N2 interface IP addresses belong to same subnet (10.10.10.0/24)
    and data path (GTP-U packets on N3 interface) is a different subnet.

    You can easily replace 10.10.10.0/24 subnet with your lab environment management network.
    For example: If your lab managment network is 10.1.1.0/24
    Then these ip addresses will be suitable for simulation.
    gnb_n1   = 10.1.1.1/24
    amf_n1   = 10.1.1.2/24
    amf_n2   = 10.1.1.3/24
    upf_n2   = 10.1.1.4/24
    gnb_n3 and upf_n3 can retain same values from 3.3.3.0/24 subnet

    Please note that if you are changing the ip addresses, you have to change the IP address
    of gnodeB inside amf/src/amf_main.c file for gnodeB registration code.
         
	

-------------------------------------------------------------------------------   
11. Steps to run the binaries 
-------------------------------------------------------------------------------   
    Open three terminal windows in linux machine. 
    -> Left side terminal executes AMF binary.
    -> Middle terminal executes AMF binary.
    -> Right side terminal executes UPF control plane binary. 
       Actual UPF is not required for NMP demonstration.
    
    
    (Use -debug option as an extra command line argument to see the parsed messages)

    1. Run AMF in middle terminal as follows:
       sudo ./amf -amfn1ip 10.10.10.2 -amfn2ip 10.10.10.3 -upfn2ip 10.10.10.4 -upfn3ip 3.3.3.1
	
    2. Run UPF in right terminal as follows:
       sudo ./upf -upfn2ip 10.10.10.4 -amfn2ip 10.10.10.3

    3. At the end, run gnodeB in left side terminal.
       ( -c option tells how many user attach requests to be simulated )
       sudo ./gnb -gnbn1ip 10.10.10.1 -gnbn3ip 3.3.3.2 -amfn1ip 10.10.10.2 -c 10
	
    You can capture the call setup packets via tcpdump on any machine. 
    (NMP message UDP port is 1208)
	
    sudo tcpdump -i <interface_name> udp port 1208 -vvxx 
	
	
-------------------------------------------------------------------------------   
12. Performance data 
-------------------------------------------------------------------------------   
   Time taken to successfully attach 65536 users (with PDN connectivity) into 
   core network is around 45 seconds with few prints.
   
   Without print statements, it takes about 30 seconds..

-------------------------------------------------------------------------------   
13. Code browsing 
-------------------------------------------------------------------------------   
   You can use "ctags" in linux. 


Thankyou so much for reading such a big file. Please contact me on my email id for any questions.
Abhishek Bhati
ab.bhati@gmail.com

