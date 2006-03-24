/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SdpHeaderS.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SDPHEADERS_H
#define SDPHEADERS_H

#include"../sdp/SdpHeader.h"

using namespace std;

class SdpHeaderS : public SdpHeader{
	public:
		SdpHeaderS(string buildFrom);
		virtual ~SdpHeaderS();
		
		virtual std::string getMemObjectType(){return "SdpHeaderS";}

		string getSessionName();
		void setSessionName(string s);

		virtual  string getString();

	private:
		string session_name;

};

#endif