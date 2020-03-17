/*
 * @Author: Yaodecheng
 * @Date: 2020-03-11 11:43:25
 * @LastEditTime: 2020-03-17 09:40:43
 * @LastEditors: Yaodecheng
 */
#include "app.h"
void
APP::run()
{}
void
APP::clear_sqe(uint32_t seq)
{
  size_t c = _respondlist.size();
  for (size_t i = 0; i < c; i++) {
    if (_respondlist[i].seq == seq) {
      _respondlist.erase(_respondlist.begin() + i);
      break;
    }
  }
}
void
APP::setCode(int ack_code, uint32_t seq)
{
  size_t count = _respondlist.size();
  for (size_t i = 0; i < count; i++) {
    if (_respondlist[i].seq == seq) {
      *_respondlist[i].code = ack_code;
      break;
    }
  }
}
std::string printf_status(int code)
{
  switch (code)
  {
  case OK:
    return "OK";
    break;
  case ERR:
    return "ERR";
    break;
  case TIMEOUT:
    return "IMEOUT";
    break;
  case InManualState:
    return "InManualState";
    break;
  case DriverIsNull:
    return "DriverIsNull";
    break;
  case ForkErr:
    return "ForkErr";
    break;

  default:
    break;
  }
  return "";
}
//        梅花
//     作者：王安石
//墙角数枝梅，凌寒独自开。
//遥知不是雪，为有暗香来
