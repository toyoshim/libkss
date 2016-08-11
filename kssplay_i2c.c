#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "kss/kss.h"
#include "kssplay.h"

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename> [<song id>]\n", argv[0]);
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

  struct timeval now, next, tick, diff;
  gettimeofday(&now, NULL);
  tick.tv_sec = 0;
  tick.tv_usec = 1000 * 1000 / 60;

  for (;;) {
    timeradd(&now, &tick, &next);
    KSSPLAY_calc_silent(kssplay, 735);
    gettimeofday(&now, NULL);
    if (timercmp(&now, &next, <)) {
      timersub(&next, &now, &diff);
      usleep(diff.tv_usec);
    }
    now = next;
  }
  return 0;
}
