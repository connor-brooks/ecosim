#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "config.h"
#include "log.h"
#include "agents.h"
#include "math.h"
#include <string.h>

Logger*
logger_create(float time, int freq)
{
  int i;
  Logger* tmp = malloc(sizeof(Logger));
  tmp->count = 5;
  for(i = 0; i < tmp->count; i++){
    tmp->log[i] = log_create();
  }
  tmp->freq = freq;
  tmp->last_log = time ;
  return tmp;
}

void logger_free(Logger* logger)
{
  int i;
  for(i = 0; i < logger->count; i++){
    log_free(logger->log[i]);
  }
  free(logger);
}

void
logger_write(Logger* logger)
{
  FILE* fp;
  fp = fopen(LOGGER_FILE, "w+");
  log_write(logger->log[LOG_TIME], fp, "x_time");
  log_write(logger->log[LOG_POPULATION], fp, "y_pop");
  log_write(logger->log[LOG_FOOD], fp, "y_food");
  log_write(logger->log[LOG_HERBIVOUR], fp, "y_herb");
  log_write(logger->log[LOG_CARNIVOUR], fp, "y_meat");
  fclose(fp);
}

void
log_write(Log* log, FILE* fp, char label[])
{
  fprintf(fp, "%s = [", label);
  fprintf(fp, "%s",log->data);
  fprintf(fp, "]\n");
}

Log* log_create()
{
  Log* tmp = malloc(sizeof(Log));
  tmp->capacity = LOG_DEFAULT_SIZE;
  tmp->data = calloc(tmp->capacity + 1, sizeof(char));
  tmp->end = 0;
  return tmp;
}

void
logger_record(Logger* logger, Agent_array* aa, float time)
{
  int i;
  int population = 0;
  int food = 0;
  int pop_herb = 0;
  int pop_carn = 0;
  int i_time = (int) time;

  if(logger->last_log + logger->freq >= time)
    return;

  logger->last_log = time;
  printf("Logged at %ds\n", i_time);

  for(i = 0; i < aa->count; i++) {
    if(aa->agents[i]->state == AGENT_STATE_LIVING) {
      population++;
      if(aa->agents[i]->dna.diet >= 0.0f)
        pop_carn++;
      else
        pop_herb++;
    }
    else
      food++;
  }
  if(i_time != 0){
    logger_log_val(logger, LOG_TIME, i_time);
    logger_log_val(logger, LOG_POPULATION, population);
    logger_log_val(logger, LOG_CARNIVOUR, pop_carn);
    logger_log_val(logger, LOG_HERBIVOUR, pop_herb);
    logger_log_val(logger, LOG_FOOD, food);
  }
}

void
logger_log_val(Logger* logger, int log_num, int n)
{
  Log_string* str = log_int_to_str(n);
  log_cat(logger->log[log_num], str);
  log_string_free(str);
}

void
log_cat(Log* log, Log_string* ls)
{
  int new_length = ls->length + log->end;
  size_t new_size;
  if(new_length >= log->capacity - 1)
  {
    log->capacity *= 2;
    new_size = sizeof(float) * log->capacity;
    log->data = realloc(log->data, new_size);
  }
  strcat(log->data + log->end, ls->string);
  log->end += ls->length - 1;
  log->data[log->end] = '\0';
}

Log_string*
log_int_to_str(int n)
{
  int i;
  Log_string* log_str = malloc(sizeof(Log_string));
  /* How many digits in the int */
  int int_char_len = log10(n) + 1;
  /*One for comma, one for space, one for nill*/
  int term_len = 3;

  log_str->length = int_char_len + term_len;
  log_str->string = calloc(term_len, sizeof(char));

  /* convert to char array */
  for(i = int_char_len; i > 0; --i) {
    log_str->string[i - 1] = 48 + (n % 10); //48 - int ascii offfset
    n /= 10;
  }

  /* Go to end and add terminators */
  i = int_char_len;
  log_str->string[i] = ',';
  i++;
  log_str->string[i] = ' ';

  return log_str;
}

void
log_string_free(Log_string* ls)
{
  free(ls->string);
  free(ls);
}

void
log_free(Log* log)
{
  free(log->data);
  free(log);
}

void log_print(Log* log);
