#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

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

  LARGE_INTEGER now, tick, next, diff;
  tick.QuadPart = 1000 * 1000 / 60;
  QueryPerformanceCounter(&now);
  for (;;) {
    next.QuadPart = now.QuadPart + tick.QuadPart;
    KSSPLAY_calc_silent(kssplay, 735);
    QueryPerformanceCounter(&now);
    if (now.QuadPart < next.QuadPart) {
      diff.QuadPart = now.QuadPart - next.QuadPart;
      Sleep(-diff.QuadPart / 1000);
    }
    now = next;
  }
  return 0;
}
