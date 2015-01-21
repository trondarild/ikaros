//
//	OscComm.cc		This file is a part of the IKAROS project
//
//    Copyright (C) 2012 <Author Name>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    See http://www.ikaros-project.org/ for more information.
//
//  This example is intended as a starting point for writing new Ikaros modules
//  The example includes most of the calls that you may want to use in a module.
//  If you prefer to start with a clean example, use he module MinimalModule instead.
//

#include "OscComm.h"
#include <sstream>
#include <iostream>
// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
OscComm::Init()
{
    channel_1_name = GetValue("channel_1_name");
    channel_2_name = GetValue("channel_2_name");
    Bind(send_port, "send_port");
    Bind(receive_port, "receive_port");
	Bind(debugmode, "debug");    

    input_array1 = GetInputArray("INPUT1");
    input_array2 = GetInputArray("INPUT2");
    input_array_size = GetInputSize("INPUT1");

    output_array1 = GetOutputArray("OUTPUT1");
    output_array2 = GetOutputArray("OUTPUT2");
    

    internal_array = create_array(10);

    //sock_send.bindTo(send_port);
    sock_send.connectTo("localhost", send_port);
    if (!sock_send.isOk()) {
        printf("OscComm: Error opening for send port %i : %s \n", send_port, sock_send.errorMessage().c_str());
    } else {
        printf("OscComm: Sender started, will send packets on port %i \n", send_port);
    }

    sock_receive.bindTo(receive_port);
    if (!sock_receive.isOk()) {
        printf("OscComm: Error opening for receive port %i : %s \n", receive_port, sock_receive.errorMessage().c_str());
    } else {
        printf("OscComm: receiver started, will receive packets on port %i \n", receive_port);
    }
}



OscComm::~OscComm()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}



void
OscComm::Tick()
{
    for (int i = 0; i < input_array_size; ++i)
    {
        
        // TODO move outside?
        std::string i_str;
        std::stringstream out;
        out << i;
        i_str = out.str();

        std::string adr1 = channel_1_name + "/" + i_str;
        std::string adr2 = channel_2_name + "/" + i_str;
        
        // send
        if (sock_send.isOk()) {

            oscpkt::Message msg1(adr1); msg1.pushFloat(input_array1[i]);
            oscpkt::Message msg2(adr2); msg2.pushFloat(input_array2[i]);
            
            //PacketWriter pw_send;
            //TODO add msg 2 when know it 
            oscpkt::PacketWriter pw;
            //pw_send.startBundle().startBundle().addMessage(msg1).endBundle().endBundle();
            //pw.startBundle().startBundle().addMessage(msg1).endBundle().endBundle();
            pw.startBundle().startBundle();
            pw.addMessage(msg1).addMessage(msg2);
            pw.endBundle().endBundle();
            //bool ok = sock_send.sendPacket(pw_send.packetData(), pw_send.packetSize());
            bool ok = sock_send.sendPacket(pw.packetData(), pw.packetSize());
            //std::cout << "Client: sent " << input_array1[i] << ", ok=" << ok << "\n";
            if(debugmode)
                printf("OscComm: sent %s, %f ok=%i\n", adr1.c_str(), input_array1[i],(int)ok);
            // wait for a reply ?
            if (sock_send.receiveNextPacket(30 /* timeout, in ms */)) {
                oscpkt::PacketReader pr(sock_send.packetData(), sock_send.packetSize());
                oscpkt::Message *incoming_msg;
                while (pr.isOk() && (incoming_msg = pr.popMessage()) != 0) {
                  printf("OscComm: Client: received %s \n ", incoming_msg->asString().c_str());
                }
            }
        }
        if(debugmode)
            printf("OscComm: sock error: %s -- is the server running?\n\n", sock_send.errorMessage().c_str());

        // receive
        if (sock_receive.isOk()) {      
            if (sock_receive.receiveNextPacket(30 /* timeout, in ms */)) {
            pr_receive.init(sock_receive.packetData(), sock_receive.packetSize());
            oscpkt::Message *msg;
            while (pr_receive.isOk() && (msg = pr_receive.popMessage()) != 0) {
              float iarg;
              if (msg->match(adr1).popFloat(iarg).isOkNoMoreArgs()) {
                output_array1[i] = iarg;
                //std::cout << "Server: received " << adr1 << iarg << " from " << sock.packetOrigin() << "\n";
                if(debugmode)
                    printf("OscComm: received %s, %f\n", adr1.c_str(), iarg);
                // oscpkt::Message repl; repl.init("/pong").pushInt32(iarg+1);
                // pw_receive.init().addMessage(repl);
                // sock_receive.sendPacketTo(pw_receive.packetData(), pw_receive.packetSize(), sock_receive.packetOrigin());
              } else if(msg->match(adr2).popFloat(iarg).isOkNoMoreArgs()) {
                output_array2[i] = iarg;
                if(debugmode)
                    printf("OscComm: received %s, %f\n", adr2.c_str(), iarg);
              } else {
                printf("OscComm: unhandled message: %s, expected %s or %s\n", msg->asString().c_str(), adr1.c_str(), adr2.c_str());
              }
            }
        }
      }

    }
	if(debugmode)
	{
		// print out debug info
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("OscComm", &OscComm::Create, "Source/UserModules/OscComm/");


