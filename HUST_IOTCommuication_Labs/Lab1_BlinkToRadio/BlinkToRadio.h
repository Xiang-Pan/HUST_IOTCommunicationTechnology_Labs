// $Id: BlinkToRadio.h,v 1.4 2006-12-12 18:22:52 vlahan Exp $

#ifndef BLINKTORADIO_H
#define BLINKTORADIO_H

enum 
{
  AM_BLINKTORADIO = 6,
  TIMER_PERIOD_MILLI = 1000,
  NODE_ID_1 = 11,
  NODE_ID_2 = 12
};

typedef nx_struct BlinkToRadioMsg 
{
  nx_uint16_t nodeid;
  nx_uint16_t counter;
}BlinkToRadioMsg;

#endif
