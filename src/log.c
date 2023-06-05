#include "../lib/log.h"
#include <stdio.h>
#include <time.h>

void log_message(const char* message) {
   time_t now;
   time(&now);
   struct tm* local_time = localtime(&now);
   char timestamp[20];
   strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);

   printf("%s - %s\n", timestamp, message);
}
