//
//	OscListener.h		This file is a part of the IKAROS project
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

#ifndef OscListener_
#define OscListener_

#include "IKAROS.h"
#include "../Osc/oscpkt.hh"
#include "../Osc/udp.hh"


class OscListener: public Module
{
public:
    static Module * Create(Parameter * p) { return new OscListener(p); }

    OscListener(Parameter * p) : Module(p) {}
    virtual ~OscListener();

    void 		Init();
    void 		Tick();
    void        ParseString(const std::string &rstr, std::vector<std::string> &radr);

    // pointers to inputs and outputs
    // and integers to represent their sizes

    //float *     input_array;
    //int         input_array_size;

    float *     output_array;
    int         output_array_size;

    // internal data storage
    float *     internal_array;
    std::vector<std::string>    adresses;

    // parameter values

    int         port;
    std::string host;
    std::string address_string;
	bool       	debugmode;

    // internals
    oscpkt::UdpSocket sock;
    oscpkt::PacketReader pr;
    oscpkt::PacketWriter pw;
};

#endif
