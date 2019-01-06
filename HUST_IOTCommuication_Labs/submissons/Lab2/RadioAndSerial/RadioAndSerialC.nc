/* FileName:    RadioAndSerialC.nc
 * Author:      Hover
 * E-Mail:      hover@hust.edu.cn
 * GitHub:      HoverWings
 * Description: RadioAndSerial Program, the module configuration and implementation
 */

/*
 * Copyright (c) 2000-2006 The Regents of the University  of California.  
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the University of California nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * Implementation of the RadioAndSerial application.  A counter is
 * incremented and a radio message is sent whenever a timer fires.
 * Whenever a radio message is received, the three least significant
 * bits of the counter in the message payload are displayed on the
 * LEDs.  Program two motes with this application.  As long as they
 * are both within range of each other, the LEDs on both will keep
 * changing.  If the LEDs on one (or both) of the nodes stops changing
 * and hold steady, then that node is no longer receiving any messages
 * from the other node.
 *
 * @author Prabal Dutta
 * @date   Feb 1, 2006
 */
#include <Timer.h>
#include "RadioAndSerial.h"

module RadioAndSerialC
{
	uses interface Boot;
	uses interface Leds;
	uses interface Timer<TMilli> as Timer0;
	uses interface Timer<TMilli> as Timer1;
	uses interface Packet;
	uses interface AMSend;
	uses interface Receive;

	uses interface Packet as SMPacket;
	uses interface Packet as SMPacket1;
	uses interface AMSend as SMSend;
	//uses interface AMSend as SMSend1;
	uses interface Receive as SMReceive;

	uses interface SplitControl as AMControl; //AMControl
	uses interface SplitControl as SMControl; //SerialControl
}

implementation 
{
	uint16_t counter;
	uint16_t nodeid;   
	message_t pkt;   		//packet
	bool busy_a = FALSE; 	//active message busy tag

	//uint16_t counter_s; 	//send counter
	message_t ppkt;  		//packet
	bool busy_s = FALSE; 	//serial message busy tag

	message_t spkt;


	event void Boot.booted() {
		call AMControl.start();
		call SMControl.start();
	}

	event void AMControl.startDone(error_t err) {
		while(err != SUCCESS) {
			call AMControl.start();
		}
	}
	event void SMControl.startDone(error_t err) {
		while(err != SUCCESS) {
			call SMControl.start();
		}
	}

	event void AMControl.stopDone(error_t err) {}
	event void SMControl.stopDone(error_t err) {}


	event void Timer0.fired() 
	{
		//call Leds.led2Off();
		if(busy_s)
		{
		RadioAndSerialMsg* btrpkt = (RadioAndSerialMsg*)(call SMPacket.getPayload(&ppkt, sizeof(RadioAndSerialMsg)));
		if (btrpkt == NULL) 
			return; 
		btrpkt->nodeid = nodeid;
		btrpkt->counter = counter;
		call SMSend.send(AM_BROADCAST_ADDR, &ppkt, sizeof(RadioAndSerialMsg));
		}
	}
	event void Timer1.fired() 
	{
		if(busy_a)
		{
		RadioAndSerialMsg* btrpkt = 
		(RadioAndSerialMsg*)(call SMPacket1.getPayload(&spkt, sizeof(RadioAndSerialMsg)));
			if (btrpkt == NULL) 
				return;
		btrpkt->nodeid = nodeid;
		btrpkt->counter = counter;
		busy_a = FALSE;
		
		//call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(RadioAndSerialMsg));
		}
	}

	event void AMSend.sendDone(message_t* msg, error_t err) 
	{
		if (&pkt == msg) {
			busy_a = FALSE;
		}
	}
	event void SMSend.sendDone(message_t* msg, error_t err) 
	{
		if(&ppkt == msg)
		{
			busy_s = FALSE;
		}
		call Leds.led0Toggle();
	}

	
	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len)
	{
		if (busy_s == TRUE) return msg;
		if (len == sizeof(RadioAndSerialMsg)) 
		{
			RadioAndSerialMsg* btrpkt = (RadioAndSerialMsg*)payload;
			nodeid = btrpkt->nodeid;
			counter = btrpkt->counter;
			//call Leds.led2On();
			call Leds.led2Toggle();
			busy_s = TRUE;
			call Timer0.startOneShot(0);
		}
		return msg;
	}

	event message_t* SMReceive.receive(message_t* smsg, void* payload, uint8_t len)
	{
		if (busy_a == TRUE) return smsg;

		if (len == sizeof(RadioAndSerialMsg)) 
		{
			RadioAndSerialMsg* btrpkt = (RadioAndSerialMsg*)payload;
			nodeid = btrpkt->nodeid;
			counter = btrpkt->counter;
			call Leds.led0Toggle();
			busy_a = TRUE;
			call AMSend.send(btrpkt->nodeid, &spkt, sizeof(RadioAndSerialMsg));
			call Timer1.startOneShot(0);
		}
		return smsg;
	}
	
}
