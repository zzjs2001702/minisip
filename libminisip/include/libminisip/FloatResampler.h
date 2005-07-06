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


/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/

#ifndef FLOAT_RESAMPLER_H
#define FLOAT_RESAMPLER_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#ifdef FLOAT_RESAMPLER

#include"Resampler.h"
#include<samplerate.h>

class LIBMINISIP_API FloatResampler : public Resampler {
	public: 

		FloatResampler( uint32_t inputFreq, uint32_t outputFreq, 
			        uint32_t duration, uint32_t nChannels );

		~FloatResampler();

		virtual void resample( short * input, short * output );

	private:
		uint32_t inputLength;
		uint32_t outputLength;

		SRC_DATA * src_data;
		SRC_STATE * src_state;

		int error;

};


#endif

#endif

