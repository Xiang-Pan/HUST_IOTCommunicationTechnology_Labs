/* FileName:    RadioAndSerial.h
 * Author:      Hover
 * E-Mail:      hover@hust.edu.cn
 * GitHub:      HoverWings
 * Description: RadioAndSerial Program, the message head
 */

#ifndef RadioAndSerial_H
#define RadioAndSerial_H

enum 
{
  AM_RadioAndSerialMSG = 6,
  TIMER_PERIOD_MILLI = 1000
};

typedef nx_struct RadioAndSerialMsg 
{
  nx_uint16_t nodeid;
  nx_uint16_t counter;
} RadioAndSerialMsg;

#endif
