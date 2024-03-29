/*
 *  UDPMessenger.cpp
 *  emptyExample
 *
 *  Created by Ryan Challinor on 5/17/11.
 *  Copyright 2011 aww bees. All rights reserved.
 *
 */

#include "UDPMessenger.h"

UDPMessenger* TheMessenger = NULL;

UDPMessenger::UDPMessenger()
{
   //assert(TheMessenger == NULL);
   TheMessenger = this;

   // open an outgoing connection to HOST:SEND_PORT
   mSender.setup( HOST, SEND_PORT );
   mSender2.setup( HOST, SEND_PORT2 );

   // open connection listening to RECEIVE_PORT
   mReceiver.setup(RECEIVE_PORT);
}

UDPMessenger::~UDPMessenger()
{
   //assert(TheMessenger == this);
   TheMessenger = NULL;
}

void UDPMessenger::Poll()
{
   while (mReceiver.hasWaitingMessages())
   {
      // get the next message
		ofxOscMessage msg;
        mReceiver.getNextMessage( msg );

      string label = msg.getAddress();

      for (std::list<Listener>::iterator i = mListeners.begin(); i != mListeners.end(); ++i)
      {
         const Listener& listener = *i;
         if (listener.mLabel == label)
            listener.mReceiver->OnMessage(msg);
      }
   }
}

void UDPMessenger::AddListener(IMessageReceiver* receiver, string label)
{
   Listener listener;
   listener.mReceiver = receiver;
   listener.mLabel = label;
   mListeners.push_back(listener);
}

void UDPMessenger::RemoveListener(IMessageReceiver* receiver)
{
   for (std::list<Listener>::iterator i = mListeners.begin(); i != mListeners.end(); ++i)
   {
      const Listener& listener = *i;
      if (listener.mReceiver == receiver)
         i = mListeners.erase(i);
   }
}

void UDPMessenger::RemoveListener(IMessageReceiver* receiver, string label)
{
   for (std::list<Listener>::iterator i = mListeners.begin(); i != mListeners.end(); ++i)
   {
      const Listener& listener = *i;
      if (listener.mLabel == label && listener.mReceiver == receiver)
         i = mListeners.erase(i);
   }
}

void UDPMessenger::SendFloatMessage(string label, float val)
{
   ofxOscMessage msg;
   msg.setAddress(label);
   msg.addFloatArg(val);
   mSender.sendMessage(msg);
   if(USE_TWO_OSC_PORTS) {
        mSender2.sendMessage(msg);
   }
}

void UDPMessenger::SendIntMessage(string label, int val)
{
   ofxOscMessage msg;
   msg.setAddress(label);
   msg.addIntArg(val);
   mSender.sendMessage(msg);
   if(USE_TWO_OSC_PORTS) {
       mSender2.sendMessage(msg);
   }
}

void UDPMessenger::SendStringMessage(string label, string val)
{
   ofxOscMessage msg;
   msg.setAddress(label);
   msg.addStringArg(val);
   mSender.sendMessage(msg);
   if(USE_TWO_OSC_PORTS) {
        mSender2.sendMessage(msg);
   }
}

void UDPMessenger::Send3DVectorMessage(string label, string name, ofVec3f val)
{
   ofxOscMessage msg;
   msg.setAddress(label);
   msg.addStringArg(name);
   msg.addFloatArg(val.x);
   msg.addFloatArg(val.y);
   msg.addFloatArg(val.z);
   mSender.sendMessage(msg);
   if(USE_TWO_OSC_PORTS) {
        mSender2.sendMessage(msg);
   }
}

void UDPMessenger::Send2DVectorMessage(string label, string name, ofVec2f val)
{
   ofxOscMessage msg;
   msg.setAddress(label);
   msg.addStringArg(name);
   msg.addFloatArg((float)(val.x*scale + xoffset));
   msg.addFloatArg((float)(val.y*scale + yoffset));
//   msg.addFloatArg(val.x);
//   msg.addFloatArg(val.y);
   mSender.sendMessage(msg);
   if(USE_TWO_OSC_PORTS) {
        mSender2.sendMessage(msg);
   }
}

void UDPMessenger::SendBooleanMessage(string label, string name, bool trigger)
{
    ofxOscMessage msg;
    msg.setAddress(label);
    msg.addStringArg(name);

    int iTrigger;
    trigger?iTrigger=1 : iTrigger=0;

    msg.addIntArg(iTrigger);
    mSender.sendMessage(msg);
   if(USE_TWO_OSC_PORTS) {
        mSender2.sendMessage(msg);
   }
}

void UDPMessenger::SetAnimataOffsets(float _xoffset, float _yoffset, float _scale) {

        xoffset = _xoffset;
        yoffset = _yoffset;
        scale = _scale;
}

