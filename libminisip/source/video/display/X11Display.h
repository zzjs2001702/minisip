/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net> 
*/

#ifndef X11_DISPLAY_H
#define X11_DISPLAY_H

#include<libminisip/libminisip_config.h>

#include<libminisip/video/ImageHandler.h>

#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include<libminisip/video/display/VideoDisplay.h>

class X11Display: public VideoDisplay{
	public: 
		X11Display( uint32_t width, uint32_t height );

		/* From ImageHandler */
		virtual void init( uint32_t height,  uint32_t width );
		virtual bool handlesChroma( uint32_t chroma );
		virtual uint32_t getRequiredWidth();
		virtual uint32_t getRequiredHeight();

        protected:
		uint32_t height;
		uint32_t width;
		uint32_t baseWindowWidth;
		uint32_t baseWindowHeight;

		virtual void openDisplay();
		virtual void createWindow();
		virtual void destroyWindow();
		virtual void handleEvents();

		virtual MImage * allocateImage();
		virtual void deallocateImage( MImage * image );

		virtual void displayImage( MImage * image );

		void toggleFullscreen();

		Display * display;
		int screen;
                int screenDepth;
		Window baseWindow;
		Window videoWindow;

		GC gc;

		bool fullscreen;

	private:

		uint32_t bytesPerPixel;
		XVisualInfo * visualInfo;

};

class X11Plugin: public VideoDisplayPlugin{
	public:
		X11Plugin( MRef<Library *> lib ): VideoDisplayPlugin( lib ){}
		
		virtual std::string getName() const { return "x11"; }

		virtual uint32_t getVersion() const { return 0x00000001; }

		virtual std::string getDescription() const { return "X11 display"; }

		virtual MRef<VideoDisplay *> create( uint32_t width, uint32_t height ) const{
			return new X11Display( width, height );
		}

		virtual std::string getMemObjectType() const { return "X11Plugin"; }
};

#endif
