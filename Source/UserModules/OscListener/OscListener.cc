//
//	OscListener.cc		This file is a part of the IKAROS project
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

#include "OscListener.h"
#include <cstring>
// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;
const std::string c_delimiter = ",";

void
OscListener::Init()
{
    printf("start\n");
    Bind(port, "port");
    //Bind(host, "host");
    host = GetValue("host");
    //host = "127.0.0.1";
    //Bind(address_string, "addresses");
    // NOTE: can be done explicit with parameters - a list of ints saying
    // size of ass. arrays
    address_string = GetValue("addresses");
	//address_string = "/1/faderA";
    Bind(debugmode, "debug");    

    //ParseString(address_string, adresses);
    
    //input_array = GetInputArray("INPUT");
    //input_array_size = GetInputSize("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");


    internal_array = create_array(10);

    //sock.connectTo(host, port);
    sock.bindTo(port);
    if (!sock.isOk()) {
        printf("OscListener: Error opening host:port %s:%i : %s \n", host.c_str(), port, sock.errorMessage().c_str());
    } else {
        printf("OscListener: Listener started, will listen to packets on port %i \n", port);
    }

}



OscListener::~OscListener()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}



void
OscListener::Tick()
{
    // dummy:
    
    if(sock.isOk())
    {
      printf("OscListener: sock is ok\n");  
      if (sock.receiveNextPacket(30 /* timeout, in ms */)) {
        printf("OscListener: packet received\n");  
        pr.init(sock.packetData(), sock.packetSize());
        oscpkt::Message *msg;
        while (pr.isOk() && (msg = pr.popMessage()) != 0) {
          printf("OscListener: going through message\n");    
          float iarg;
          // TODO do proper matching against parameters etc
          if (msg->match(address_string).popFloat(iarg).isOkNoMoreArgs()) {
            output_array[0] = iarg;
            printf("OscListener: received /lfo %f from %s\n", iarg, sock.packetOrigin().asString().c_str());
            //oscpkt::Message repl; repl.init("/pong").pushInt32(iarg+1);
            //pw.init().addMessage(repl);
            //sock.sendPacketTo(pw.packetData(), pw.packetSize(), sock.packetOrigin());
          } else {
            printf("OscListener: unhandled message: %s\n", msg->asString().c_str() );
            //cout << "Server: unhandled message: " << *msg << "\n";
          }
        }
      }  
    }
    else
        printf("OscListener: sock is not ok\n");  
	if(debugmode)
	{
		// print out debug info
	}
}


void        
OscListener::ParseString(const std::string &rstr, std::map<std::string, float*> &radr)
{
    //parse the string
    char *pch = strtok (const_cast<char*>(rstr.c_str()), c_delimiter.c_str());
    while (pch != NULL)
    {
        //printf ("%s\n",pch);
        char *str = pch;//strtok(NULL, c_namenum_sep.c_str());
        char *num  =strtok(NULL, c_delimiter.c_str()); 
        //std::cout << "got " << str << ", " << num << "\n";
        float *array = new float[atoi(num)];
        radr.insert(std::pair<std::string, float*> (std::string(str), array));
        // next
        pch = strtok (NULL, c_delimiter.c_str());
    }
}


/*
void runServer() {
  UdpSocket sock; 
  sock.bindTo(PORT_NUM);
  if (!sock.isOk()) {
    cerr << "Error opening port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
  } else {
    cout << "Server started, will listen to packets on port " << PORT_NUM << std::endl;
    PacketReader pr;
    PacketWriter pw;
    while (sock.isOk()) {      
      if (sock.receiveNextPacket(30 )) {
        pr.init(sock.packetData(), sock.packetSize());
        oscpkt::Message *msg;
        while (pr.isOk() && (msg = pr.popMessage()) != 0) {
          int iarg;
          if (msg->match("/ping").popInt32(iarg).isOkNoMoreArgs()) {
            cout << "Server: received /ping " << iarg << " from " << sock.packetOrigin() << "\n";
            Message repl; repl.init("/pong").pushInt32(iarg+1);
            pw.init().addMessage(repl);
            sock.sendPacketTo(pw.packetData(), pw.packetSize(), sock.packetOrigin());
          } else {
            cout << "Server: unhandled message: " << *msg << "\n";
          }
        }
      }
    }
  }
}
*/



// Install the module. This code is executed during start-up.

static InitClass init("OscListener", &OscListener::Create, "Source/UserModules/OscListener/");


