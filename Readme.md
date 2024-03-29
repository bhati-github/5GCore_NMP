# README

# Table of Contents<!-- omit in toc -->

- [1. Aim of this Project](#aim-of-this-project)
- [2. Motivation behind idea and implementation of this protocol ](#Motivation-behind-idea-and-implementation-of-this-protocol)
- [3. Fundamental Idea behind NMP](#Fundamental-Idea-behind-NMP)
- [4. Explaination of NMP (Network Message Protocol) header](#Explaination-of-NMP-(Network-Message-Protocol)-header)
- [5. Explaination of items to be carried by NMP message](#Explaination-of-items-to-be-carried-by-NMP-message)
- [6. Demonstration in a sample network](#Demonstration-in-a-sample-network)
  - [6.1  Simulation of NG Setup with AMF in NMP packets](#Simulation-of-NG-Setup-with-AMF-in-NMP-packets)
    - [6.1.1  Parsed Message from gnodeB to AMF (NG Setup Request)](#Parsed-Message-from-gnodeB-to-AMF-NG-Setup-Request)
    - [6.1.2  Parsed Message from AMF to gnodeB (NG Setup Response)](#Parsed-Message-from-AMF-to-gnodeB-NG-Setup-Response)
  - [6.2 Simulation of Initial UE Registration Message and PDU Setup Request in NMP packets](#Simulation-of-Initial-UE-Registration-Message-and-PDU-Setup-Request-in-NMP-packets)
  - [6.3 Simulation of PFCP packet flows in NMP packets](#Simulation-of-PFCP-packet-flows-in-NMP-packets)
    - [6.3.1 Parsed Message from SMF to UPF (Session Create Request)](#Parsed-Message-from-SMF-to-UPF-Session-Create-Request)
    - [6.3.2 Parsed Message from UPF to SMF (Session Create Response)](#Parsed-Message-from-UPF-to-SMF-Session-Create-Response)
  - [6.4 Simulation of NF Registration request (AMF -> NRF) in NMP packets](#Simulation-of-NF-Registration-request-in-NMP-packets)
- [7. Steps for simulation inside virtual machine](#Steps-for-simulation-inside-virtual-machine)
- [8. Performance data ](#Performance-data)

<br />
<br />

## 1. Aim of this Project
-   This project demonstrate the use of newly created NMP (Network Message Protocol) 
    which can be used for information exchange between telecommunication network nodes.
-   Simulate NGAP procedure packet flows over NMP protocol instead of SCTP protocol.
-   Simulate N4 interface packet flows over NMP protocol instead of PFCP protocol.
-   Simulate Namf-Nnrf, Nsmf-Nnrf communication over NMP protocol instead of HTTP2 REST.


<br />
<br />
  
  
## 2. Motivation behind idea and implementation of this protocol
-  During development of S1AP protocol parser and 5GNAS protocol parser, the complexity in the parser leads me to think about a new protocol which can be parsed very easily and can be used across all call setup related interfaces.
   So, this leads to idea of Network Message Protocol (NMP) and eventually this demonstration project.

-  It is known that HTTP REST based APIs are used currently in 5G core network for inter-node communications. HTTP REST (Representational State Transfer) concept is
   to send complete state even if you wish to modify a single 2 byte element in target node. So, for just trying to communicate a 2 byte change in state, you are
   transferring entire state (in KB) towards target node. Imagine if your state grows to 10 MB in size for some reason, are you going to send your complete state
   of 10 MB (spanning across multiple fragmented packets) towards target node? This protocol serves as an alternative for such issues..  

<br />
<br />
    
    
    
## 3. Fundamental Idea behind NMP 
    
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

<br />

![item-description](https://github.com/bhati-github/5GCore_NMP/assets/49303596/135d223f-5b81-4bc8-9a60-de012a3bb654)

<br />

    ////////////////////////////////////////////////////////
    // Item id's carrying 1 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__MSG_RESPONSE,
    ITEM_ID__UPLINK_QOS_PROFILE,
    ITEM_ID__DNLINK_QOS_PROFILE,
    ITEM_ID__PDR_ACTION,
    ITEM_ID__FAR_ACTION_FLAGS,
    ITEM_ID__FAR_DST_INTERFACE,
    ITEM_ID__DEFAULT_PAGING_DRX,
    ITEM_ID__RELATIVE_AMF_CAPACITY,

    ////////////////////////////////////////////////////////
    // Item id's carrying 2 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__RAN_UE_NGAP_ID,
    ITEM_ID__AMF_UE_NGAP_ID,
    ITEM_ID__PDR_RULE_ID,
    ITEM_ID__PDR_PRECEDENCE,
    ITEM_ID__PDR_FAR_ID,
    ITEM_ID__FAR_RULE_ID,
    ITEM_ID__RRC_ESTABLISH_CAUSE,

    ////////////////////////////////////////////////////////
    // Item id's carrying more than 2 and upto 4 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__MCC_MNC,
    ITEM_ID__GNODEB_ID,
    ITEM_ID__TAC,
    ITEM_ID__CELL_ID,
    ITEM_ID__TMSI,
    ITEM_ID__UE_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_GTPU_TEID,

    ////////////////////////////////////////////////////////
    // Item id's carrying more than 4 and upto 8 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__IMSI,
    ITEM_ID__NSSAI,
    ITEM_ID__GTPU_SELF_IPV4_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV4_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE,
    ITEM_ID__USER_LOCATION_INFO_TAC,

    ////////////////////////////////////////////////////////
    // Item id's carrying more than 8 and upto 16 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__UE_IDENTIFIER_SECRET,
    ITEM_ID__UE_IPV6_ADDR,
    ITEM_ID__USER_LOCATION_INFO_NR_CGI,

    ////////////////////////////////////////////////////////
    // Also known as Type-2 items.
    // Item id's carrying more than 16 byte value (variable length)
    // 2 bytes(item_id)  +
    // 2 bytes(item_len) +
    // actual item bytes
    ////////////////////////////////////////////////////////
    ITEM_ID__GTPU_SELF_IPV6_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV6_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE,
    ITEM_ID__NAS_PDU,
    ITEM_ID__RAN_NODE_NAME,
    ITEM_ID__AMF_NAME,
    ITEM_ID__GUAMI,

    ////////////////////////////////////////////////////////
    // Item id's carrying group of individual items
    // Item group can contain any possible combination
    // of type-1 and type-2 items.
    // Think about concept of recursion.
    //
    // 2 bytes(group_item_id) +
    // 2 bytes(item_count)    +
    // 2 bytes(item_len)      +
    // actual item bytes
    ////////////////////////////////////////////////////////
    ITEM_GROUP_ID__N3_PDR,
    ITEM_GROUP_ID__N6_PDR,
    ITEM_GROUP_ID__N3_FAR,
    ITEM_GROUP_ID__N6_FAR,
    ITEM_GROUP_ID__USER_LOCATION_INFO,
    ITEM_GROUP_ID__GLOBAL_RAN_NODE_ID,
    ITEM_GROUP_ID__GUAMI_LIST,
    ITEM_GROUP_ID__SUPPORTED_TA_LIST,
    ITEM_GROUP_ID__SUPPORTED_TA_LIST_ITEM,
    ITEM_GROUP_ID__PLMN_SUPPORT_LIST,
    ITEM_GROUP_ID__PLMN_SUPPORT_LIST_ITEM,

<br />
<br />

## 6. Demonstration in a sample network
<br />

  ![vm-network-visio](https://github.com/bhati-github/5GCore_NMP/assets/49303596/74a6c76e-6c9d-412e-bc81-32ff03666682)


<br />
This sample network is created inside a virtual machine using "dummy interfaces".

Steps to create these interfaces and simulating call flows over NMP are described in detail later in this document.
<br />

- N1/N2 interface is used for call setup packets between gnodeB and AMF.
  As part of this demonstration, all messages on this interface are carried by NMP protocol.
   
- N4 interface is used for datapath setup inside UPF. (SMF <---> UPF).
   As part of this demonstration, all messages on this interface are carried by NMP protocol.
   
- N3 interface carry data packets of UE via GTP-U packets. User plane GTP-U packet flows are not part of this demonstration. 
   
- N6 interface is towards Internet. De-capsulated GTP-U packets towards Internet are not part of this demonstration.
   
- Overall, colored blocks in above diagram are implemented and all of them communicates over NMP protocol
  except N3 and N6 interface for obvious reasons as these interfaces are meant to carry user data packets.
  
<br />

![ue-0-logs](https://github.com/bhati-github/5GCore_NMP/assets/49303596/e4c703bc-3475-49fa-98f5-669ba4c76401)

<br />


### 6.1 Simulation snapshots of NG Setup with AMF in NMP packets
  ![ng-setup](https://github.com/bhati-github/5GCore_NMP/assets/49303596/2cbb3e06-1140-4b83-96b5-3582695f2124)
<br />
  
#### 6.1.1 Parsed Message from gnodeB to AMF (NG Setup Request Message over NMP protocol)
<br />

 ![ng-setup-1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/3223d198-642c-4c27-8776-81f46cb7170a)
 ![ng-setup-2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/abf5f0d5-a501-47d5-be38-3fd1fd46b254)

<br />

#### 6.1.2 Parsed Message from AMF to gnodeB (NG Setup Response over NMP protocol)
<br />

 ![ng-setup-response-1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/4dcbe230-9dea-4b57-a0ed-b3e92b58dee4)
 ![ng-setup-response-2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/316ca72e-32b9-437b-9766-6ad059f658f8)

<br />
 
 
### 6.2 Simulation snapshots of Initial UE Registration Message and PDU Setup Request in NMP packets
  
  ![sequence](https://github.com/bhati-github/5GCore_NMP/assets/49303596/1ed44f2b-759b-46b0-bfa2-e6f98ce87231)

  ![ng-setup-and-ue-reg](https://github.com/bhati-github/5GCore_NMP/assets/49303596/b38e2843-d3ac-4e19-a84a-8a7c2f3880ad)

 
Above pcap snapshot is taken from a real 5G core network. As you can see, SCTP is carrying NGAP procedure messages.
<br />
In below snapshots, you can see that same NGAP procedures are carried over NMP protocol.
<br />

 ![1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/f1cbac95-7f5c-40e6-acc0-4fb6e2944e5b)
 ![2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/d9f71c8e-dafe-4eb7-a85b-c586209bd6de)
 ![3](https://github.com/bhati-github/5GCore_NMP/assets/49303596/0ad2edb5-2be4-4436-b74c-f5a725df395e)
 ![4](https://github.com/bhati-github/5GCore_NMP/assets/49303596/1beb2055-f810-42f5-a1ce-b68570742112)
 ![5](https://github.com/bhati-github/5GCore_NMP/assets/49303596/8fb9b8f8-df03-4506-8daf-9bd9560c2ca3)
 ![6](https://github.com/bhati-github/5GCore_NMP/assets/49303596/9288fdf7-3926-4ab6-b3ec-488d6b670e6b)
 ![7](https://github.com/bhati-github/5GCore_NMP/assets/49303596/1d38b069-8dcd-4e64-b051-6a65cd3cabf8)
 ![8](https://github.com/bhati-github/5GCore_NMP/assets/49303596/dd41ce54-f940-490d-a065-f22d71b2bb1b)
 ![9](https://github.com/bhati-github/5GCore_NMP/assets/49303596/3f3f5c2e-74a2-4e1f-a414-b6e47e9e4ca6)

<br />
<br />

### 6.3 Simulation snapshots of PFCP packet flows in NMP packets 
N4 interface between SMF and UPF carries PFCP protocol packets for user-plane parameter setup.

Following simulation carries same information in NMP packets as depicted in screenshots.

In this simulation, SMF is integrated within AMF and this is the reason that N4 packet is originating from AMF N4 interface.

<br />

##### 6.3.1 Parsed Message from SMF to UPF (Session Create Request) 
<br />

![1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/81be6394-3e49-4dfe-9360-8d7315a9611d)
![2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/fcc865f2-1319-4c1c-9cad-66222410d3e3)
![3](https://github.com/bhati-github/5GCore_NMP/assets/49303596/e29f9b1a-447c-4d82-81d2-1df5dd85f45e)
![4](https://github.com/bhati-github/5GCore_NMP/assets/49303596/6c1c8fa6-b947-43cf-905a-4e3786c4e243)

<br />

##### 6.3.2 Parsed Message from UPF to SMF (Session Create Response) 
<br />

![resp-1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/253814d0-6ead-4210-a75c-afe298814bdc)
![resp-2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/b23994f1-c5be-4e1e-bd08-082f5f863125)

<br />

### 6.4 Simulation snapshots of NF Registration request (AMF --> NRF) over NMP protocol 
<br />

 ![1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/728e2263-0030-4bca-bfe8-e4ad9a000447)
 ![2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/b76cb5d0-5592-48a0-a270-b100d84cc8e5)

<br />



## 7. Steps for simulation inside virtual machine

First step is to create dummy interfaces inside VM using these commands.

<br />

    #// For gnodeB N1/N2
    sudo ip link add eth1 type dummy
    sudo ip addr add 2.2.2.1/24 dev eth1
    sudo ip link set eth1 up

    #// For gnodeB N3
    sudo ip link add eth2 type dummy
    sudo ip addr add 3.3.3.1/24 dev eth2
    sudo ip link set eth2 up
    
    #// For AMF N1/N2
    sudo ip link add eth3 type dummy
    sudo ip addr add 2.2.2.2/24 dev eth3
    sudo ip link set eth3 up
    
    #// For AMF Namf
    sudo ip link add eth4 type dummy
    sudo ip addr add 5.5.5.1/24 dev eth4
    sudo ip link set eth4 up

    #// For SMF Nsmf
    sudo ip link add eth5 type dummy
    sudo ip addr add 5.5.5.2/24 dev eth5
    sudo ip link set eth5 up
    
    #// For SMF N4
    sudo ip link add eth6 type dummy
    sudo ip addr add 4.4.4.1/24 dev eth6
    sudo ip link set eth6 up
    
    #// For UPF N3
    sudo ip link add eth7 type dummy
    sudo ip addr add 3.3.3.2/24 dev eth7
    sudo ip link set eth7 up
    
    #// For UPF N4
    sudo ip link add eth8 type dummy
    sudo ip addr add 4.4.4.2/24 dev eth8
    sudo ip link set eth8 up
    
    #// For UPF N6
    sudo ip link add eth9 type dummy
    sudo ip addr add 6.6.6.1/24 dev eth9
    sudo ip link set eth9 up
    
    #// For AF Naf
    sudo ip link add eth10 type dummy
    sudo ip addr add 5.5.5.3/24 dev eth10
    sudo ip link set eth10 up
    
    #// For AUSF Nausf
    sudo ip link add eth11 type dummy
    sudo ip addr add 5.5.5.4/24 dev eth11
    sudo ip link set eth11 up
    
    #// For NRF Nnrf
    sudo ip link add eth12 type dummy
    sudo ip addr add 5.5.5.5/24 dev eth12
    sudo ip link set eth12 up
   
    #// For NSSF Nnssf
    sudo ip link add eth13 type dummy
    sudo ip addr add 5.5.5.6/24 dev eth13
    sudo ip link set eth13 up
   
    #// For NEF Nnef
    sudo ip link add eth14 type dummy
    sudo ip addr add 5.5.5.7/24 dev eth14
    sudo ip link set eth14 up
   
    #// For PCF Npcf
    sudo ip link add eth15 type dummy
    sudo ip addr add 5.5.5.8/24 dev eth15
    sudo ip link set eth15 up

    #// For UDM Nudm
    sudo ip link add eth16 type dummy
    sudo ip addr add 5.5.5.9/24 dev eth16
    sudo ip link set eth16 up

    You can copy paste above commands into create_5g_network.sh file and simply execute this bash script inside your vm. 
    
<br />
    
    Now, open five terminal windows from same vm. 
    -> 1st terminal executes 'gnb' binary.
    -> 2nd terminal executes 'nrf' binary.
    -> 3rd terminal executes 'amf' binary.
    -> 4th terminal executes 'smf' binary.
    -> 5th terminal executes 'upf' control plane binary. Actual UPF is not required for NMP demonstration.
    
    (Use -debug option as an extra command line argument to see the parsed messages)

    1. Run NRF in 2nd terminal.
       cd 5GCore_NMP/nrf/
       make clean;make
       sudo ./nrf -Nnrfip 5.5.5.5 -Namfip 5.5.5.1  -Nsmfip 5.5.5.2

       -Nnrfip 5.5.5.5   (NRF Nnrf interface IP address)
       -Namfip 5.5.5.1   (AMF Namf interface IP address)
       -Nsmfip 5.5.5.2   (SMF Nsmf interface IP address)
       
    2. Run AMF in 3rd terminal as follows:
       cd 5GCore_NMP/amf/
       make clean;make
       sudo ./amf -myn1n2ip 2.2.2.2 -myNamfip 5.5.5.1 -Nnrfip 5.5.5.5 -Nsmfip 5.5.5.2 -gnbreg 2.2.2.1 3.3.3.1

       In above command, options are as follows:
       -myn1n2ip 2.2.2.2         (AMF N1/N2 interface IP address)
       -myNamfip 5.5.5.1         (AMF Namf interface IP address)
       -Nnrfip 5.5.5.5           (NRF Nnrf interface IP address)
       -Nsmfip 5.5.5.2           (SMF Nsmf interface IP address)
       -gnbreg 2.2.2.1 3.3.3.1  (Register a gnodeB into AMF with its N1/N2 interface and N3 interface details)
                                             
    3. Run SMF in 4th terminal as follows:
       cd 5GCore_NMP/smf/
       make clean;make
       sudo ./smf -myn4ip 4.4.4.1 -myNsmfip 5.5.5.2 -Namfip 5.5.5.1 -Nnrfip 5.5.5.5 -upfn4ip 4.4.4.2 -upfn3ip 3.3.3.2

       In above command, options are as follows:
       -myn4ip 4.4.4.1       (SMF N4 interface IP address)
       -myNsmfip 5.5.5.2     (SMF Nsmf interface IP address)
       -Namfip 5.5.5.1       (AMF Namf interface IP address)
       -Nnrfip 5.5.5.5       (NRF Nnrf interface IP address)
       -upfn4ip 4.4.4.2      (UPF N4 interface IP address)
       -upfn3ip 3.3.3.2      (UPF N3 interface IP address)
       
    4. Run UPF in 5th terminal as follows:
       cd 5GCore_NMP/upf/
       make clean;make
       sudo ./upf -myn3ip 3.3.3.2 -myn4ip 4.4.4.2 -smfn4ip 4.4.4.1 -myn6ip 6.6.6.1

       In above command, options are as follows:
       -myn3ip 3.3.3.2     (UPF N3 interface IP address)
       -myn4ip 4.4.4.2     (UPF N4 interface IP address)
       -smfn4ip 4.4.4.1    (SMF N4 interface IP address)
       -myn6ip 6.6.6.1     (UPF N6 interface IP address)
       

    5. Finally, run gnb in 1st terminal. 
       sudo ./gnb -myn1n2ip 2.2.2.1 -myn3ip 3.3.3.1 -amfn1n2ip 2.2.2.2 -c 1
       or
       sudo ./gnb -myn1n2ip 2.2.2.1 -myn3ip 3.3.3.1 -amfn1n2ip 2.2.2.2 -c 1 -debug
       
       In above command, options are as follows:
       -myn1n2ip 2.2.2.1     (gnodeB N1/N2 interface IP address)
       -myn3ip  3.3.3.1      (gnodeB N3 interface IP address)
       -amfn1n2ip 2.2.2.2    (AMF N1/N2 interface IP address)
       -c 10  (Simmulate upto 10 UE attach requests)
       -debug (Show complete NMP message parsing) 
	
    You can capture the call setup packets via tcpdump (NMP message UDP port is 1208)
    sudo tcpdump -i <interface_name> udp port 1208 -vvxx 

<br />
<br />
<br />

## 8. Performance data 
   Time taken to successfully attach 65536 users (with PDN connectivity) into 
   core network is around 45 seconds with few prints. Without print statements, it takes about 30 seconds..

<br />
<br />


Thankyou so much for reading such a big file. Please contact me on my email id for any questions.

<br />
<br />

If you liked this demostration project, please give it a "star" and spread in your circle.

<br />
<br />

Abhishek Bhati
<br />
ab.bhati@gmail.com
<br />
https://www.linkedin.com/in/abhishek-bhati-2166b15a/
<br />
<br />
