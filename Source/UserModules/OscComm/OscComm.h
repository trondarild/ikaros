//
//	OscComm.h		This file is a part of the IKAROS project
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

#ifndef OscComm_
#define OscComm_
#include "../Osc/oscpkt.hh"
#include "../Osc/udp.hh"

#include "IKAROS.h"

class OscComm: public Module
{
public:
    static Module * Create(Parameter * p) { return new OscComm(p); }

    OscComm(Parameter * p) : Module(p) {}
    virtual ~OscComm();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array1;
    float *     input_array2;
    int         input_array_size;

    float *     output_array1;
    float *     output_array2;

    // internal data storage
    float *     internal_array;

    // parameter values

    std::string channel_1_name;
    std::string channel_2_name;
    int         send_port;
    int         receive_port;
	bool       	debugmode;
    char        i_buf[10];

    // internals
    oscpkt::UdpSocket sock_send;
    oscpkt::PacketReader pr_send;
    oscpkt::PacketWriter pw_send;

    oscpkt::UdpSocket sock_receive;
    oscpkt::PacketReader pr_receive;
    oscpkt::PacketWriter pw_receive;
};

#endif
