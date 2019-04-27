#ifndef LOG_H
#define LOG_H
#include <stddef.h>
#include "agents.h"
/* For debug / graphing */

typedef struct _Log Log;
typedef struct _Logger Logger;
typedef struct _Log_axis Log_axis;
typedef struct _Log_string Log_string;

enum logger_logs {
  LOG_TIME,
  LOG_POPULATION,
  LOG_FOOD,
  LOG_HERBIVOUR,
  LOG_CARNIVOUR,
  LOG_METABOLISM_AVG,
  LOG_VISION_AVG,
  LOG_REBIRTH_AVG,
  LOG_DIET_AVG,
  LOG_FLOCK_AVG,
  LOG_WOBBLE_AVG
};

struct _Logger {
Log* log[11];
int count;
float last_log;
int freq;
};

struct _Log_string {
  char* string;
  size_t length;
};

struct _Log {
  char* data;
  int capacity;
  ptrdiff_t end;
};


Logger* logger_create(float time, int freq);
void logger_free(Logger* logger);
void logger_record(Logger* logger, Agent_array* aa, float time);
void logger_log_val(Logger* logger, int log_num, int x);
void logger_write(Logger* logger);

Log* log_create();
void log_free(Log* log);
void log_cat(Log* log, Log_string* ls);
void log_write(Log* log, FILE* fp, char label[]);
void log_print(Log* log);

Log_string* log_int_to_str(int n);
void log_string_free(Log_string* ls);

#endif
