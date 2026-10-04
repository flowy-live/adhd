#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sqlite3.h>

struct state *s = NULL;

#define INTENTION_MAX_SIZE 2048

typedef struct record_t {
  char *key;
  U32 value;
} Record;

struct state {
  Mode mode;
  char intention[INTENTION_MAX_SIZE];
  Record **records; // array of records
  size_t size_records;
  size_t capacity_records;
};

static sqlite3 *db = NULL;

static void db_open() {
  int r = sqlite3_open("local.sqlite3", &db);
  if (r == SQLITE_OK) {
  } else {
    fprintf(stderr, "Unable to prepare sqlite db\n");
    exit(-1);
  }

  const char *sql = 
  "CREATE TABLE IF NOT EXISTS records ("
    "key TEXT PRIMARY KEY,"
    "value INT"
    ");"
    ;

  char *err;
  r = sqlite3_exec(db, sql, NULL, NULL, &err);
  if (r != SQLITE_OK) {
    fprintf(stderr, "SQL error during setup: %s\n", err);
    sqlite3_free(err);
    sqlite3_close(db);
    exit(-1);
  }
}

static void db_close() {
  if (db) {
    sqlite3_close(db);
  }
}

static int db_get_record_callback(void *ctx, int argc, char *argv[], char **argv_col_name) {
  Record *rec = calloc(1, sizeof(Record));
	 
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv_col_name[i], "key") == 0) {
      rec->key = strdup(argv[i]);
    } else if (strcmp(argv_col_name[i], "value") == 0) {
      rec->value = (U32) atoi(argv[i]);
    }
  }

  if (s->size_records >= s->capacity_records) {
    printf("MAX capacity");
    return 0;
  }
  
  *(s->records + s->size_records) = rec;
  s->size_records++;
  
  return 0;
}

static void db_upsert_record(char *key, U32 value) {
  sqlite3_stmt *stmt;
  const char *sql = "INSERT INTO records (key, value) VALUES (?, ?) ON CONFLICT (key) DO UPDATE SET value = ?;";
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  assert(rc == SQLITE_OK);
  sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, value);
  sqlite3_bind_int(stmt, 3, value);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE) {
    printf("Failure in insert: %s\n", sqlite3_errmsg(db));
  }

  sqlite3_finalize(stmt);
}

static void db_get_all_records() {
  const char *sql = "SELECT key, value FROM records;";
  int rc = sqlite3_exec(db, sql, db_get_record_callback, NULL, NULL);
  assert(rc == SQLITE_OK);
}

void init_state() {
  s = malloc(sizeof(struct state));
  s->mode = WINDOW;

  memset(s->intention, 0, sizeof(s->intention));

  s->size_records = 0;
  s->capacity_records = 1000; //TODO: make dynamic
  s->records = malloc(sizeof(Record *) * s->capacity_records);

  db_open();
  db_get_all_records();
}

static Record *append_record(char *key, U32 value) {
  if (s->size_records >= s->capacity_records) {
    printf("MAX capacity");
    return NULL;
  }
  
  char *cpy = strdup(key);

  Record *new_record = malloc(sizeof(Record));
  new_record->key = cpy;
  new_record->value = value;
  
  *(s->records + s->size_records) = new_record;
  s->size_records++;
  return new_record;
}

static void upsert_record(char *key, U32 value) {
  bool found = false;
  for (int i = 0; i < s->size_records; i++) {
    Record *rec = *(s->records + i);
    if (strcmp(rec->key, key) == 0) {
      found = true;
      rec->value = value;
      break;
    }
  }

  if (!found) {
    append_record(key, value);
  }
}

static Record *get_record(char *key) {
  for (int i = 0; i < s->size_records; i++) {
    Record *rec = *(s->records + i);
    if (strcmp(rec->key, key) == 0) {
      return rec;
    }
  }

  return NULL;
}

static void print_records() {
  for (int i = 0; i < s->size_records; i++) {
    Record *rec = *(s->records + i);
    printf("record: %s -> %d\n", rec->key, rec->value);
  }
}

static void generate_key(struct tm *time, char *buffer, size_t buffer_size) {
  snprintf(buffer, buffer_size, "%d", time->tm_yday);
}

void cleanup_state() {
  if (!s) {
    return;
  }

  for (int i = 0; i < s->size_records; i++) {
    Record *rec = *(s->records + s->size_records);
    free(rec);
  }
  free(s->records);
  
  free(s);
  s = NULL;

  db_close();
}

void set_intention(char *intention) {
  assert(s);
  
  strncpy(s->intention, intention, sizeof(s->intention));
  s->intention[sizeof(s->intention) - 1] = '\0';
}

char *get_intention() {
  assert(s);
  return s->intention;
}

size_t get_intention_size() {
  assert(s);
  return sizeof(s->intention);
}

Mode get_mode() {
  assert(s);
  return s->mode;
}

void set_mode(Mode new_mode) {
  s->mode = new_mode;
}

U32 get_result(struct tm *time) {
  char key[10];
  generate_key(time, key, sizeof(key));
  Record *rec = get_record(key);
  if (!rec) {
    return 0;
  }
  
  return rec->value;
}

void set_result(struct tm *time, U32 result) {
  char key[10];
  generate_key(time, key, sizeof(key));
  upsert_record(key, result);
  db_upsert_record(key, result);
}
