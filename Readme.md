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
    - [6.3.1 Parsed Message from AMF+SMF to UPF (Session Create Request)](#Parsed-Message-from-AMF+SMF-to-UPF-Session-Create-Request)
    - [6.3.2 Parsed Message from UPF to AMF+SMF (Session Create Response)](#Parsed-Message-from-UPF-to-AMF+SMF-Session-Create-Response)
- [7. Network Diagram for simulation inside a single virtual machine](#Network-Diagram-for-simulation-inside-a-single-virtual-machine)
- [8. Performance data ](#Performance-data)

<br />
<br />

## 1. Aim of this Project
-   This project demonstrate the use of newly created NMP (Network Message Protocol) 
    which can be used for information exchange between telecommunication network nodes.
-   Simulate NGAP procedure flows over NMP protocol instead of SCTP protocol
-   Simulate N4 interface packet flows over NMP protocol instead of PFCP protocol
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

-  Use of SCTP protocol can be bypassed with this protocol for NGAP procedures between gnodeB and AMF.
-  Use of PFCP protocol can be bypassed with this protocol between SMF and UPF.

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

   ![1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/9291faeb-cd9a-4750-86a7-ea6617e783e0)


   The code inside this demonstration project illustrates the use of NMP protocol 
   across sample interfaces in this diagram. 

- N1/N2 interface is used for call setup packets between gnodeB and AMF.
  As part of this demonstration, all messages on this interface are carried by NMP protocol.
   
- N4 interface is used for datapath setup inside UPF. (SMF <---> UPF).
   As part of this demonstration, all messages on this interface are carried by NMP protocol
   
   
- N3 interface carry data packets of UE via GTP-U packets. User plane GTP-U packet flows are not part of this demonstration. 
   
- N6 interface is towards Internet. De-capsulated GTP-U packets towards Internet are not part of this demonstration.
   

- There is no separate SMF in this diagram as session management function is integrated within AMF for simplicity of demonstration.

<br />
<br />


### 6.1 Simulation of NG Setup with AMF in NMP packets
  ![ng-setup](https://github.com/bhati-github/5GCore_NMP/assets/49303596/2cbb3e06-1140-4b83-96b5-3582695f2124)

  ![ng-setup-pcap](https://github.com/bhati-github/5GCore_NMP/assets/49303596/586614ca-1358-4477-9701-ca66fe9ac9df)

<br />
<br />
  
#### 6.1.1 Parsed Message from gnodeB to AMF (NG Setup Request)

  ![ng-setup-1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/b13e95be-abef-45ec-93bd-5c2fe214d3ab)
  ![ng-setup-2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/ebd2dcd3-2aa7-4a78-be63-ee66fc091fe4)

<br />
<br />

#### 6.1.2 Parsed Message from AMF to gnodeB (NG Setup Response)

  ![ng-setup-response-1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/63badf4a-e355-4b1a-8d13-a483f85c20fa)
  ![ng-setup-response-2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/29eb922e-8e0f-46be-aea3-32e07d6dd68a)

<br />
<br />
<br />
  
 
### 6.2 Simulation of Initial UE Registration Message and PDU Setup Request in NMP packets
  
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
<br />
<br />

### 6.3 Simulation of PFCP packet flows in NMP packets 
N4 interface between SMF and UPF carries PFCP protocol packets for user-plane parameter setup.

Following simulation carries same information in NMP packets as depicted in screenshots.

In this simulation, SMF is integrated within AMF and this is the reason that N4 packet is originating from AMF N4 interface.

<br />

##### 6.3.1 Parsed Message from AMF+SMF to UPF (Session Create Request) 

![1](https://github.com/bhati-github/5GCore_NMP/assets/49303596/a96f6064-d2ec-42d7-aaed-cdc7c2e3a694)
![2](https://github.com/bhati-github/5GCore_NMP/assets/49303596/f9978191-cbcf-41a9-bbe9-7dfe08fb2f64)
![3](https://github.com/bhati-github/5GCore_NMP/assets/49303596/d30408fa-e15a-4e01-9d85-ad1533d18875)
![4](https://github.com/bhati-github/5GCore_NMP/assets/49303596/cc4163bf-69cb-4109-8849-c095894c6cab)
![5](https://github.com/bhati-github/5GCore_NMP/assets/49303596/b4d7f962-a04b-48a8-8759-dd6b2a5f30d3)

<br />

##### 6.3.2 Parsed Message from UPF to AMF+SMF (Session Create Response) 

![6](https://github.com/bhati-github/5GCore_NMP/assets/49303596/4765a4e4-a7c4-4fd9-8a0e-79a0bf7609c0)

<br />
<br />
<br />
<br />

## 7. Network Diagram for simulation inside a single virtual machine

<br />

![vm-network](https://github.com/bhati-github/5GCore_NMP/assets/49303596/ae57c728-fbfa-4ab2-b9b1-2f5d0c77ec78)

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
       sudo ./amf -amfn1n2ip 192.168.1.16 -amfn4ip 192.168.1.17 -upfn4ip 192.168.1.18 -upfn3ip 192.168.1.18 -gnbreg 192.168.1.15 192.168.1.15

       In above command, options are as follows:
       -amfn1n2ip 192.168.1.16 (AMF N1/N2 interface IP address is 192.168.1.16)
       -amfn4ip 192.168.1.17 (AMF N4 interface IP address is 192.168.1.17)
       -upfn4ip 192.168.1.18 (UPF N4 interface IP address is 192.168.1.18)
       -upfn3ip 192.168.1.18 (UPF N3 interface IP address is 192.168.1.18)
       -gnbreg 192.168.1.15 192.168.1.15  (Register a gnodeB into AMF with its N1/N2 interface and N3 interface details)
                                             You can simulate AMF instance with multiple gnodeB's also.

       For example, if you wish to simulate AMF with 3 gnodeB's. Then provide -gnbreg options as follows:
       -gnbreg 192.168.1.21 192.168.1.21 -gnbreg 192.168.1.22 192.168.1.22 -gnbreg 192.168.1.23 192.168.1.23 


    2. Run UPF in right terminal as follows:
       cd 5GCore_NMP/upf/
       make clean;make
       sudo ./upf -upfn4ip 192.168.1.18 -amfn4ip 192.168.1.17

       In above command, options are as follows:
       -upfn4ip 192.168.1.18 (UPF N4 interface IP address is 192.168.1.18)
       -amfn4ip 192.168.1.17 (AMF N4 interface IP address is 192.168.1.17)
       

    3. At the end, run gnodeB in left side terminal. ( -c option tells how many user attach requests to be simulated )
       sudo ./gnb -gnbn1n2ip 192.168.1.15 -gnbn3ip 192.168.1.15 -amfn1n2ip 192.168.1.16 -c 1
       or
       sudo ./gnb -gnbn1n2ip 192.168.1.15 -gnbn3ip 192.168.1.15 -amfn1n2ip 192.168.1.16 -c 1 -debug

       In above command, options are as follows:
       -gnbn1n2ip 192.168.1.15 (gnodeB N1/N2 interface IP address is 192.168.1.15)
       -gnbn3ip 192.168.1.15   (gnodeB N3 interface IP address is 192.168.1.15)
       -amfn1n2ip 192.168.1.16 (AMF N1/N2 interface IP address is 192.168.1.16)
       -c 10  (Simmulate upto 10 UE attach requests)
       -debug (Show complete NMP message parsing) 
	
    You can capture the call setup packets via tcpdump (NMP message UDP port is 1208)
    sudo tcpdump -i <interface_name> udp port 1208 -vvxx 

<br />
<br />
<br />

## 8. Performance data 
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
