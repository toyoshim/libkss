#include <stdio.h>
#include <string.h>
#include <usb.h>

#include "emu2149.h"
#include "emu2212.h"

//#define ENABLE_WRITE_CHECK

static usb_dev_handle* i2c = NULL;

void sc_init() {
  if (i2c)
    return;

  usb_init();
  usb_find_busses();
  usb_find_devices();
  for (struct usb_bus* bus = usb_get_busses(); bus; bus = bus->next) {
    for (struct usb_device* dev = bus->devices; dev; dev = dev->next) {
      if (dev->descriptor.idVendor == 0x0403 &&
          dev->descriptor.idProduct == 0xc631) {
        i2c = usb_open(dev);
        break;
      }
    }
  }
  if (!i2c) {
    fprintf(stderr, "i2c-tiny-usb: not found\n");
    return;
  }
  const int cmd_delay = 2;
  const int delay = 0;
  if (usb_control_msg(
        i2c, USB_TYPE_VENDOR, cmd_delay, delay, 0, NULL, 0, 1000) < 0) {
    fprintf(stderr, "i2c-tiny-usb: delay configuration failed: %s\n",
        usb_strerror());
  }
}

void PSG_set_quality(PSG* psg, uint32_t quality) {}
int16_t PSG_calc(PSG* psg) { return 0; }
void PSG_setVolumeMode(PSG* psg, int mode) {}
uint32_t PSG_setMask(PSG* psg, uint32_t mask) { return 0; }
void PSG_reset(PSG* psg) {}
void PSG_delete(PSG* psg) {}

static PSG psg;

PSG* PSG_new(uint32_t c, uint32_t r) {
  sc_init();
  memset(&psg, 0, sizeof(psg));
  return &psg;
}

void PSG_writeIO(PSG* psg, uint32_t adr, uint32_t val) {
  if (!i2c)
    return;

  if (adr & 1) {
    if (psg->adr > 15)
      return;
    const int out = USB_TYPE_CLASS;
    const int cmd_wr = 7;
    char msg[2];
    msg[0] = psg->adr;
    msg[1] = val;
    if (usb_control_msg(i2c, out, cmd_wr, 0, 0x50, msg, 2, 10) < 1) {
      fprintf(stderr, "i2c-tiny-usb: %s\n", usb_strerror());
      return;
    }
#if defined(ENABLE_WRITE_CHECK)
    const int in = USB_TYPE_CLASS | USB_ENDPOINT_IN;
    const int cmd_rc = 3;
    char rc;
    if (usb_control_msg(i2c, in, cmd_rc, 0, 0, &rc, 1, 10) < 0 || rc != 1) {
      fprintf(stderr, "i2c-tiny-usb: %s\n", usb_strerror());
      return;
    }
#endif
  } else {
    psg->adr = val & 0x1f;
  }
}

uint8_t PSG_readIO(PSG* psg) { return psg->reg[psg->adr]; }

static SCC scc;

void SCC_set_quality(SCC* scc, uint32_t q) {}
int16_t SCC_calc(SCC* scc) { return 0; }
void SCC_set_type(SCC* scc, uint32_t type) {}
uint32_t SCC_setMask(SCC* scc, uint32_t adr) { return 0; }
void SCC_reset(SCC* scc) {}
void SCC_delete(SCC* scc) {}

SCC* SCC_new(uint32_t c, uint32_t r) {
  sc_init();
  memset(&scc, 0, sizeof(scc));
  return &scc;
}

static void SCC_write_sc(uint8_t addr, uint8_t val) {
  const int out = USB_TYPE_CLASS;
  const int cmd_wr = 7;
  char msg[2];
  msg[0] = addr;
  msg[1] = val;
  if (usb_control_msg(i2c, out, cmd_wr, 0, 0x51, msg, 2, 10) < 1) {
    fprintf(stderr, "i2c-tiny-usb: %s\n", usb_strerror());
    return;
  }
#if defined(ENABLE_WRITE_CHECK)
  const int in = USB_TYPE_CLASS | USB_ENDPOINT_IN;
  const int cmd_rc = 3;
  char rc;
  if (usb_control_msg(i2c, in, cmd_rc, 0, 0, &rc, 1, 10) < 0 || rc != 1) {
    fprintf(stderr, "i2c-tiny-usb: %s\n", usb_strerror());
    return;
  }
#endif
}

void SCC_write(SCC* scc, uint32_t addr, uint32_t val) {
  if (!i2c)
    return;

  if (addr & 0xf000 == 0xb000)
    scc->type = SCC_ENHANCED;
  else
    scc->type = SCC_STANDARD;

  if (scc->type == SCC_ENHANCED)
    return SCC_write_sc(addr & 0xff, val & 0xff);

  addr &= 0xff;
  if (addr >= 0xe0)
    return;
  if (addr >= 0x80)
    return SCC_write_sc((addr + 0x20) & 0xff, val & 0xff);
  if (addr >= 0x60) {
    SCC_write_sc((addr + 0x20) & 0xff, val & 0xff);
  }
  SCC_write_sc(addr & 0xff, val & 0xff);
}
