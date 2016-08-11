#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>

#include "kss/kss.h"
#include "kssplay.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename> [<song id>]\n", argv[0]);
    return 1;
  }

  int dsp = open("/dev/dsp", O_RDWR);
  const int channels = 2;
  const int frequency = 44100;
  const int format = AFMT_S16_LE;
  if (dsp < 0 ||
      ioctl(dsp, SNDCTL_DSP_CHANNELS, &channels) < 0 ||
      ioctl(dsp, SNDCTL_DSP_SPEED, &frequency) < 0 ||
      ioctl(dsp, SNDCTL_DSP_SETFMT, &format) < 0) {
    perror("/dev/dsp initialization");
    return 1;
  }
  KSSPLAY* kssplay = KSSPLAY_new(44100, 2, 16);

  KSS* kss = KSS_load_file(argv[1]);
  if (NULL == kss) {
    fprintf(stderr, "%s: KSS_load_file failed\n", argv[1]);
    return 1;
  }

  KSSPLAY_set_data(kssplay, kss);
  KSSPLAY_reset(kssplay, 2 < argc ? atoi(argv[2]) : 0, 0);

  uint16_t buffer[735 * 2];
  for (;;) {
    KSSPLAY_calc(kssplay, buffer, 735);
    write(dsp, buffer, 735 * 4);
  }
  return 0;
}
