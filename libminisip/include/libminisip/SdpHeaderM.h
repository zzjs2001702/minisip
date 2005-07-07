/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SdpHeaderM.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SDPHEADERM_H
#define SDPHEADERM_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<libminisip/SdpHeader.h>
#include<vector>

#ifdef _MSC_VER
typedef uint32_t unsigned int;
#else
#include<inttypes.h>
#endif


class SdpHeaderA;

class LIBMINISIP_API SdpHeaderM : public SdpHeader{
	public:
		SdpHeaderM(std::string buildFrom);
		SdpHeaderM(std::string media, int32_t port, int32_t n_ports, std::string transport);
		virtual ~SdpHeaderM();
		
		virtual std::string getMemObjectType(){return "SdpHeaderM";}

		void addFormat(int32_t format);
		int32_t getNrFormats();
		int32_t getFormat(int32_t i);

		std::string getMedia();
		void setMedia(std::string m);

		int32_t getPort();
		void setPort(int32_t p);

		int32_t getNrPorts();
		void setNrPorts(int32_t n);

		std::string getTransport();
		void setTransport(std::string t);

		virtual std::string getString();

		void addAttribute(MRef<SdpHeaderA*>);
		std::string getAttribute(std::string key, uint32_t index);
		std::list<MRef<SdpHeaderA*> > getAttributes();

		std::string getRtpMap(uint32_t format);

	private:
		std::string media;
		int32_t port;
		int32_t nPorts;
		std::string transport;
		std::vector<int32_t> formats;
		std::list<MRef<SdpHeaderA*> >attributes;
};

#endif