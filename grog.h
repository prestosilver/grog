#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_ON_FNS 420420

typedef void (*grug_runtime_error_handler_t)(char *reason, enum grug_runtime_error_type type, char *on_fn_name, char *on_fn_path);
typedef void (*grug_init_globals_fn_t)(void *globals, uint64_t id);

struct grog_file {
  void *memory;
  grug_init_globals_fn_t initGlobalsFn;
  size_t globals_size;
  size_t entities_size;
  size_t resources_size;

  void *functions[MAX_ON_FNS];
  size_t functions_size;
};

struct grog_file *grog_open(char *path);
char **grog_get_resources(struct grog_file *file);
void *grog_symbol(struct grog_file *file, char *name);
bool grog_close(struct grog_file *file);

extern char grog_error[256];
