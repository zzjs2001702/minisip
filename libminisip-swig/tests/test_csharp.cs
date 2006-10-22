/*
 Copyright (C) 2006  Mikael Magnusson

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Library General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* Copyright (C) 2006
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

using System;
using System.Threading;
using System.Runtime.InteropServices;
using Minisip;

public class MyDbgHandler : DbgHandler {
  public delegate void displayMessageDelegate(string output, int style);
  private string prefix;

  private displayMessageDelegate displayMessageMethod;

  public MyDbgHandler(displayMessageDelegate method, string prefix) {
    displayMessageMethod = method;
    this.prefix = prefix;
  }

  protected override void displayMessage(string output, int style) {
    if ( output != "" )
      displayMessageMethod(prefix + style + " " + output, style);
  }
}

public class MyGui : GuiProxy {
//   private DbgBuf outBuf;
//   private DbgBuf errBuf;

//   private streambuf oldOutBuf;
//   private streambuf oldErrBuf;
  private string callId;

  //  private SipSoftPhoneConfigurationRef config;

  public MyGui() {
    Console.WriteLine("MyGui");

    Dbg.getOut().setEnabled(true);
    Dbg.getErr().setEnabled(true);
    //Dbg.getDbg().setEnabled(true);

//     Dbg.getOut().setExternalHandler(new MyDbgHandler(displayMessage, "mout"));
//     Dbg.getErr().setExternalHandler(new MyDbgHandler(displayMessage, "merr"));
//     Dbg.getDbg().setExternalHandler(new MyDbgHandler(displayMessage, "mdbg"));

//     outBuf = new DbgBuf(new MyDbgHandler(displayMessage, "out"));
//     errBuf = new DbgBuf(new MyDbgHandler(displayMessage, "err"));

//     oldOutBuf = MinisipModule.setOutputBuf(outBuf);
//     oldErrBuf = MinisipModule.setErrorBuf(errBuf);
  }

  public void stopStreams() {
    Console.WriteLine("MyGui stop");
//     MinisipModule.setOutputBuf(oldOutBuf);
//     MinisipModule.setErrorBuf(oldErrBuf);
  }

  public override void run() {
    for(;;){
      //Console.WriteLine("Before waitCommand");
      CommandString cmd = waitCommand();
      //Console.WriteLine("After waitCommand");
      if( cmd.getOp() == "stop" ){
	//Console.Writ	eLine("Return thread");
	return;
      }
      handleCommand( cmd );
    }
  }

  protected virtual void displayMessage(string output, int style) {
    //System.Windows.Forms.MessageBox.Show(output);
    Console.Write(output);
  }

  public override void setSipSoftPhoneConfiguration(SipSoftPhoneConfigurationRef config) {
    if( config.isNull() ) {
      //Console.WriteLine("setSipSoftPhoneConfiguration NULL!");
    } else {
      Console.WriteLine("setSipSoftPhoneConfiguration");
      if( !config.pstnIdentity.isNull() )
	dumpIdentity(config.pstnIdentity);
      dumpIdentities(config.identities);
    }
  }

  public override void handleCommand(CommandString command) {
    Console.WriteLine("My handleCommand: " + command.getString());

    if( command.getOp()=="invite_ok" ){
      CommandString sendersOn = new CommandString(callId, MediaCommandString.set_session_sound_settings, "senders", "ON");
      sendCommand("media", sendersOn);

      Console.WriteLine("Activated media");
    }
  }

    public override bool configDialog(SipSoftPhoneConfigurationRef conf) {
    Console.WriteLine("configDialog");
    return true;
  }

  private void dumpIdentity(SipIdentityRef id){
    Console.WriteLine("id     " + id.identityIdentifier);
    Console.WriteLine("id:    " + id.getId());
    Console.WriteLine("uri:   " + id.getSipUri());
    Console.WriteLine("name   " + id.sipUsername);
    Console.WriteLine("domain " + id.sipDomain);
    Console.WriteLine("sec    " + id.securitySupport);
    Console.WriteLine("reg    " + id.registerToProxy);
    Console.WriteLine("reg?   " + id.isRegistered());

    dumpProxy(id.getSipProxy());
  }

  private void dumpIdentities(SipIdentityList list) {
    System.Collections.IEnumerator ien = list.GetEnumerator();
    ien.Reset();
    while(ien.MoveNext()) {
      SipIdentityRef id = (SipIdentityRef)ien.Current;
      dumpIdentity(id);
    }
  }

  private void dumpProxy(SipProxyRef proxy){
    Console.WriteLine("SipProxy");
    Console.WriteLine("addr      " + proxy.sipProxyAddressString);
    Console.WriteLine("port      " + proxy.sipProxyPort);
    Console.WriteLine("username  " + proxy.sipProxyUsername);
    Console.WriteLine("transport " + proxy.getTransport());
  }

  public override void setContactDb(ContactDbRef contactDb) {
    Console.WriteLine("setContactDb");    
  }

  public void sendInvite(string uri){
    CommandString invite = new CommandString("", SipCommandString.invite, uri);
    CommandString resp = getCallback().handleCommandResp("sip", invite);
    Console.WriteLine("Calling: " + resp.getString());
    callId = resp.get("destination_id");
  }
}


public class runme {
  static void Main() {
    MinisipModule.setupDefaultSignalHandling();

    Console.WriteLine("MiniSIP C# GUI ... welcome!");

    MyGui gui = new MyGui();
    GuiRef guiref = new GuiRef(gui);
    GC.SuppressFinalize(gui);

    ThreadStart job = new ThreadStart( gui.run );
    Thread thread = new Thread( job );
    thread.Start();

    MinisipMain sip = new MinisipMain(guiref, 0, null);
    

    if( sip.startSip() <= 0 ){
      return;
    }

    Console.WriteLine("SIP started");

//     CommandString register;
//     register = new CommandString("", SipCommandString.proxy_register);
//     register.setKey("proxy_domain", "fowley.hem.za.org");
//     cb.handleCommand("sip", register);

//     gui.sendInvite("sip:xxx@yyy");
    
    //gui.run();

    Console.WriteLine("<<< Press ENTER to quit >>>");
    Console.ReadLine();

    gui.stopStreams();
    gui.stop();
    thread.Join();
    sip.exit();
    Console.WriteLine("Bye");
    return;
  }
}