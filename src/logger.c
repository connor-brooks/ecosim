#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "config.h"
#include "logger.h"
#include "agents.h"
#include "math.h"
#include <string.h>

Logger*
logger_create(float time, int freq)
{
  int i;
  Logger* tmp = malloc(sizeof(Logger));
  tmp->count = 11;
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
  log_write(logger->log[LOG_METABOLISM_AVG], fp, "y_metab_a");
  log_write(logger->log[LOG_VISION_AVG], fp, "y_vision_a");
  log_write(logger->log[LOG_REBIRTH_AVG], fp, "y_rebirth_a");
  log_write(logger->log[LOG_DIET_AVG], fp, "y_diet_a");
  log_write(logger->log[LOG_FLOCK_AVG], fp, "y_flock_a");
  log_write(logger->log[LOG_WOBBLE_AVG], fp, "y_wobble_a");
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
  int i_time = (int) time;
  int total[logger->count];
  Agent* tmp_agent;

  if(logger->last_log + logger->freq >= time) return;


  logger->last_log = time;
  printf("Logged at %ds\n", i_time);

  /* Null out totals array */
  for(i = 0; i < logger->count; i++)
    total[i] = 0;

  /* save the time */
  total[LOG_TIME] = (int) time;

  /* return if time is zero */
  if(total[LOG_TIME] == 0) return;

  for(i = 0; i < aa->count; i++) {
    tmp_agent = aa->agents[i];
    /* Log living agents */
    if(tmp_agent->state == AGENT_STATE_LIVING) {
      total[LOG_POPULATION]++;

      /* log trait */
      total[LOG_METABOLISM_AVG] += tmp_agent->dna.metabolism * 100;
      total[LOG_VISION_AVG] += tmp_agent->dna.vision * 100;
      total[LOG_REBIRTH_AVG] += tmp_agent->dna.rebirth * 100;
      total[LOG_DIET_AVG] += tmp_agent->dna.diet * 100;
      total[LOG_FLOCK_AVG] += tmp_agent->dna.flock * 100;
      total[LOG_WOBBLE_AVG] += tmp_agent->dna.wobble * 100;

      /* Carnivour or plant */
      if(agent_diet(tmp_agent) == AGENT_DIET_LIVING)
        total[LOG_CARNIVOUR]++;
      else
        total[LOG_HERBIVOUR]++;
    }
    /* Dead agents are food */
    else if(tmp_agent->state == AGENT_STATE_DEAD)
      total[LOG_FOOD]++;
  }

  /* Get trait averages */
  total[LOG_METABOLISM_AVG] /= total[LOG_POPULATION];
  total[LOG_VISION_AVG] /= total[LOG_POPULATION];
  total[LOG_REBIRTH_AVG] /= total[LOG_POPULATION];
  total[LOG_DIET_AVG] /= total[LOG_POPULATION];
  total[LOG_FLOCK_AVG] /= total[LOG_POPULATION];
  total[LOG_WOBBLE_AVG] /= total[LOG_POPULATION];

  /* change five to count */
  for(i = 0; i < logger->count; i++) {
    logger_log_val(logger, i, total[i]);
  }
  logger_write(logger);
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
  int int_char_len;
  Log_string* log_str = malloc(sizeof(Log_string));

  /* How many digits in the int */
  if(n != 0)
    int_char_len = log10(n) + 1;
  else
    int_char_len = 1;

  /*One for comma, one for space, one for nill*/
  int term_len = 3;

  log_str->length = int_char_len + term_len;
  log_str->string = calloc(term_len, sizeof(char));

  /* Go to end and add terminators */
  i = int_char_len;
  log_str->string[i] = ',';
  i++;
  log_str->string[i] = ' ';

  /* If the value was zero, don't do math,
   * just add the char '0' to array and return*/
  log_str->string[int_char_len - 1] = 48;
  if(n == 0) return log_str;

  /* otherwise convert to char array */
  for(i = int_char_len; i > 0; --i) {
    log_str->string[i - 1] = 48 + (n % 10); //48 - int ascii offfset
    n /= 10;
  }

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
