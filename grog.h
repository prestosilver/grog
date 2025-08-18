#pragma once

#include "grug.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_ON_FNS 420420

//// Structs

struct grog_file {
  void *memory;
  size_t memory_size;

  size_t globals_size;
  size_t entities_size;
  size_t resources_size;

  void **on_functions;
  grug_init_globals_fn_t init_globals_fn;
};

struct grog_file *grog_open(char *path);
char **grog_get_resources(struct grog_file *file, size_t *size);
char **grog_get_entities(struct grog_file *file, size_t *size);
char *grog_get_entity_type(struct grog_file *file, size_t index);
bool grog_close(struct grog_file *file);

extern char grog_error[256];
