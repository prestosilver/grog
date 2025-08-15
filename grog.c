#include "grog.h"

#include <iso646.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#ifdef __MINGW32__
#include <windows.h>
HMODULE GetCurrentModule()
{ // NB: XP+ solution!
  HMODULE hModule = NULL;
  GetModuleHandleEx(
    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
    (LPCTSTR)GetCurrentModule,
    &hModule);

  return hModule;
}

void *GetSymbol(HMODULE module, char *sym) {
  return GetProcAddress(module, sym);
}

void *GetWriteMemory(size_t size) {
  void *buffer = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);

  return buffer;
}

void MakeExecMemory(void *buffer, size_t size) {
  DWORD dummy;
  bool ok = VirtualProtect(buffer, size, PAGE_EXECUTE_READ, &dummy);
}

#else
#include <sys/mman.h>

void *GetCurrentModule() {
  return dlopen(NULL, RTLD_NOW);
}

void *GetSymbol(void *module, char *sym) {
  return dlsym(module, sym);
}

void *GetWriteMemory(size_t size) {
  return mmap(0, size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}

void MakeExecMemory(void *memory, size_t size) {}
#endif

char grog_error[256];

#define grog_ERROR(...) { \
  snprintf(grog_error, sizeof(grog_error), __VA_ARGS__); \
}

struct __attribute__((packed)) grog_header {
  uint32_t magic;
  bool *grug_has_runtime_error_happened;
  bool *grug_on_fns_in_safe_mode;
  char **grug_fn_path;
  char **grug_fn_name;
  uint32_t entities_size;
  uint32_t resources_size;
  uint32_t globals_size;
  uint32_t init_globals_len;
};

extern bool grug_has_runtime_error_happened;
extern bool grug_on_fns_in_safe_mode;
extern char *grug_fn_path;
extern char *grug_fn_name;

void empty_on_fn() {
  return;
}

struct grog_file *grog_open(char *path) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    grog_ERROR("file %s dosent exist", path);
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  int size = ftell(file);

  char *mapped = GetWriteMemory(size);

  fseek(file, 0L, SEEK_SET);
  fread(mapped, 1, size, file);
  fclose(file);

  struct grog_header *header = (struct grog_header *)(mapped);

  header->grug_has_runtime_error_happened = &grug_has_runtime_error_happened;
  header->grug_on_fns_in_safe_mode = &grug_on_fns_in_safe_mode;
  header->grug_fn_path = &grug_fn_path;
  header->grug_fn_name = &grug_fn_name;

  struct grog_file *result = malloc(sizeof(struct grog_file));

  void *root = GetCurrentModule();

  void *init_globals = (void *)(&mapped[sizeof(struct grog_header)]);

  size_t extern_functions_count = *(uint32_t*)(&mapped[header->init_globals_len + sizeof(struct grog_header)]);
  char *current_data_ptr = (char *)(&mapped[header->init_globals_len + sizeof(struct grog_header) + 4]);
  while (extern_functions_count--) {
    int len = strlen(&current_data_ptr[4]);
    void * symbol = GetSymbol(root, &current_data_ptr[4]);

    *((void **)(&current_data_ptr[len + 4])) = symbol;
    current_data_ptr += 4 + len + sizeof(void *);
  }

  *result = (struct grog_file){
    .memory = mapped,
    .init_globals_fn = init_globals,
    .globals_size = header->globals_size,
    .entities_size = header->entities_size,
    .resources_size = header->resources_size,
  };

  for (int i = 0; i < MAX_ON_FNS; i ++) {
    result->on_functions[i] = &empty_on_fn;
  }

  uint32_t strings_count = *(uint32_t*)(current_data_ptr);
  current_data_ptr += 4;
  while (strings_count--) {
    uint32_t data_len = *(uint32_t*)(current_data_ptr);
    current_data_ptr += 4 + data_len;
  }

  uint32_t on_fns_count = *(uint32_t*)(current_data_ptr);
  current_data_ptr += 4;
  while (on_fns_count--) {
    uint32_t name_len = *(uint32_t*)(current_data_ptr);
    char *fn_name = current_data_ptr + 4;
    current_data_ptr += 4 + name_len;

    uint32_t fn_index = *(uint32_t*)(current_data_ptr);
    current_data_ptr += 4;

    uint32_t data_len = *(uint32_t*)(current_data_ptr);
    char *fn_data = current_data_ptr + 4;
    current_data_ptr += 4 + data_len;

    result->on_functions[fn_index] = fn_data;
  }

  MakeExecMemory(mapped, size);

  return result;
}

char **grog_get_resources(struct grog_file *file, size_t *size) {
  *size = 0;
  return NULL;
}

char **grog_get_entities(struct grog_file *file, size_t *size) {
  *size = 0;
  return NULL;
}

char *grog_get_entity_type(struct grog_file *file, size_t index) {
  return NULL;
}

bool grog_close(struct grog_file *file) {
  munmap(file->memory, file->memory_size);
  free(file);

  return false;
}
