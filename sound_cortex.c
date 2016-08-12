#include <stdio.h>
#include <string.h>
#include "libusb.h"

#include "emu2149.h"

// For compatibility hack to link with old static library, libusb-1.0.lib.
FILE _imp___iob[3];

void PSG_set_quality(PSG* psg, uint32_t quality) {}
int16_t PSG_calc(PSG* psg) { return 0; }
void PSG_setVolumeMode(PSG* psg, int mode) {}
uint32_t PSG_setMask(PSG* psg, uint32_t mask) { return 0; }

static PSG psg;
static libusb_device_handle* i2c = NULL;
static int instance = 0;

PSG *PSG_new (uint32_t c, uint32_t r) {
  if (!i2c) {
    // Do like this, but I do not have enough confidence if this works
    // because FILE structure might be changed.
    _imp___iob[0] = *stdin;
    _imp___iob[1] = *stdout;
    _imp___iob[2] = *stderr;
    memset(&psg, 0, sizeof(psg));

    libusb_init(NULL);
    i2c = libusb_open_device_with_vid_pid(NULL, 0x0403, 0xc631);
    if (!i2c) {
      fprintf(stderr, "i2c-tiny-usb: not found\n");
      return &psg;
    }
    const int cmd_delay = 2;
    const int delay = 0;
    if (libusb_control_transfer(
        i2c, LIBUSB_REQUEST_TYPE_VENDOR, cmd_delay, delay, 0, NULL, 0, 1000) < 0) {
      fprintf(stderr, "i2c-tiny-usb: delay configuration failed\n");
    }
  }
  instance++;
  return &psg;
}

void PSG_reset(PSG* psg) {
  for (int r = 0; r < 14; ++r) {
    PSG_writeIO(psg, 0, r);
    PSG_writeIO(psg, 1, 0);
  }
}

void PSG_delete(PSG* psg) {
  instance--;
  if (!instance) {
    libusb_close(i2c);
    i2c = NULL;
    libusb_exit(NULL);
  }
}

void PSG_writeIO(PSG* psg, uint32_t adr, uint32_t val) {
  if (adr & 1) {
    if (psg->adr > 15)
      return;
    const int out = LIBUSB_REQUEST_TYPE_CLASS;
    const int cmd_wr = 7;
    char msg[2];
    msg[0] = psg->adr;
    msg[1] = val;
	if (libusb_control_transfer(i2c, out, cmd_wr, 0, 0x50, msg, 2, 1000) < 1) {
      fprintf(stderr, "i2c-tiny-usb: control transfer failed to write\n");
      return;
    }
    const int in = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_ENDPOINT_IN;
    const int cmd_rc = 3;
    char rc;
    if (libusb_control_transfer(i2c, in, cmd_rc, 0, 0, &rc, 1, 1000) < 0 ||
        rc != 1) {
      fprintf(stderr, "i2c-tiny-usb: control transfer failed to read\n");
      return;
    }
  } else {
    psg->adr = val & 0x1f;
  }
}

uint8_t PSG_readIO(PSG* psg) { return psg->reg[psg->adr]; }
