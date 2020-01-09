/* prointellicolor - Util for setting taillight color for the Microsoft Pro IntelliMouse.
 * Copyright (C) 2019  namazso
 *
 *             DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <stdio.h>
#include <stdlib.h>

#include "hidapi.h"

hid_device* open_device(unsigned short vendor_id, unsigned short product_id, unsigned short usage)
{
  hid_device* dev = NULL;
  struct hid_device_info *devs, *cur_dev;
  devs = hid_enumerate(vendor_id, product_id);
  cur_dev = devs;
  while (cur_dev)
  {
    if (cur_dev->usage == usage)
    {
      dev = hid_open_path(cur_dev->path);
      break;
    }
    cur_dev = cur_dev->next;
  }
  hid_free_enumeration(devs);

  return dev;
}

int set_color(hid_device* dev, unsigned char r, unsigned char g, unsigned char b)
{
  unsigned char buf[73];

  memset(buf, 0, sizeof(buf));

  buf[0] = 0x24;
  buf[1] = 0xB2;
  buf[2] = 0x03;
  buf[3] = (unsigned char)r;
  buf[4] = (unsigned char)g;
  buf[5] = (unsigned char)b;
  
  return hid_send_feature_report(dev, buf, sizeof(buf));
}

int main(int argv, char** argc)
{
  int r, g, b;
  hid_device* dev;
  int res, ret = 0;

  if(argv < 4)
  {
    fprintf(stderr, "usage: prointellicolor <r> <g> <b>\n");
    ret = 1;
    goto free_none;
  }

  r = atol(argc[1]);
  g = atol(argc[2]);
  b = atol(argc[3]);

  if(r > 255 || g > 255 || b > 255)
  {
    fprintf(stderr, "error: value out of bounds\n");
    ret = 2;
    goto free_none;
  }

  if (hid_init())
  {
    fprintf(stderr, "error: cannot initialize hidapi\n");
    ret = 3;
    goto free_none;
  }

  dev = open_device(0x045E, 0x082A, 0x0212);
  if(!dev)
  {
    fprintf(stderr, "error: cannot open device\n");
    ret = 4;
    goto free_hid;
  }

  res = set_color(dev, r, g, b);
  if (res < 0)
  {
    fprintf(stderr, "error: cannot write device: %ws\n", hid_error(dev));
    ret = 5;
    goto free_dev;
  }

free_dev:
  hid_close(dev);

free_hid:
  hid_exit();

free_none:
  return ret;
}
