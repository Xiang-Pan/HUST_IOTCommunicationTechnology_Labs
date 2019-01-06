/* FileName:    BlinkToRadioAppC.nc
 * Author:      Hover
 * E-Mail:      hover@hust.edu.cn
 * GitHub:      HoverWings
 * Description: BlinkToRadioC Program, the module configuration and implementation
 */

// $Id: BlinkToRadioC.nc,v 1.6 2010-06-29 22:07:40 scipio Exp $

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
 * Implementation of the BlinkToRadio application.  A counter is
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
#include "BlinkToRadio.h"

module BlinkToRadioC {
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

	uses interface SplitControl as AMControl; //无线控制
	uses interface SplitControl as SMControl; //串口控制
}
implementation {

	uint16_t counter;
	uint16_t nodeid;
	uint16_t aim_node = 1;   
	message_t pkt;   //数据包
	bool busy_a = FALSE; //节点无线忙标志

	//uint16_t counter_s; //发送次数
	message_t spkt;  //数据包
	bool busy_s = FALSE; //串口忙标志

	void setLeds(uint16_t val) {
		if (val & 0x01)
			call Leds.led0On();
		else 
			call Leds.led0Off();
		if (val & 0x02)
			call Leds.led1On();
		else
			call Leds.led1Off();
		if (val & 0x04)
			call Leds.led2On();
		else
			call Leds.led2Off();
	}
	void LedsOff() {
		call Leds.led0Off();
		call Leds.led1Off();
		call Leds.led2Off();
	}

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
		LedsOff();
		if(busy_s)
		{
			
		}
	}
	event void Timer1.fired() 
	{
		LedsOff();
		if(busy_a)
		{
		
		}
	}

	event void AMSend.sendDone(message_t* msg, error_t err) 
	{
		if (&spkt == msg) {
			busy_a = FALSE;
		}
	}
	event void SMSend.sendDone(message_t* msg, error_t err) 
	{
		if(&pkt == msg)
		{
			busy_s = FALSE;
		}
	}

	
	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len)
    {
		if (busy_s == TRUE) return msg;
		if (len == sizeof(BlinkToRadioMsg)) 
		{
			BlinkToRadioMsg* btrpkt = (BlinkToRadioMsg*)payload;
			BlinkToRadioMsg* trpkt = (BlinkToRadioMsg*)(call SMPacket.getPayload(&pkt, sizeof(BlinkToRadioMsg)));
			if(btrpkt->nodeid == TOS_NODE_ID) 
			{
				trpkt->counter = btrpkt->counter + TOS_NODE_ID;
				trpkt->nodeid = btrpkt->nodeid;
				busy_s = TRUE;
				setLeds(btrpkt->counter);
				call SMSend.send(trpkt->nodeid, &pkt, sizeof(BlinkToRadioMsg));
				call Timer0.startOneShot(3000);
			}
		}
		return msg;
	}

	event message_t* SMReceive.receive(message_t* smsg, void* payload, uint8_t len)
    {
		if (busy_a == TRUE) return smsg;
		if (len == sizeof(BlinkToRadioMsg)) 
        {
			BlinkToRadioMsg* btrpkt = (BlinkToRadioMsg*)payload;
			BlinkToRadioMsg* trpkt = (BlinkToRadioMsg*)(call SMPacket1.getPayload(&spkt, sizeof(BlinkToRadioMsg)));      
			trpkt->nodeid = btrpkt->nodeid;
			trpkt->counter = btrpkt->counter;
			setLeds(trpkt->nodeid);
			busy_a = TRUE;
			call AMSend.send(trpkt->nodeid, &spkt, sizeof(BlinkToRadioMsg));
			call Timer1.startOneShot(1000);
		}
		return smsg;
	}
	
}
