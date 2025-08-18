struct grog_extern_fn {
    u32 data_len;
    char name[data_len];
    u64 ptr;
};

struct on_fn {
    u32 name_len;
    char name[name_len];
    u32 fn_index;
    u32 data_len;
    char data[data_len];
};

struct grog_string {
    u32 name_len;
    char name[name_len];
};
    
struct grog_data {
    u64 grug_has_runtime_error_happened;
    u64 grug_on_fns_in_safe_mode;
    u64 grug_fn_path;
    u64 grug_fn_name;
};

struct grog_file {
    u32 MAGIC;
    grog_data pad;
    u32 globals_size;
    u32 entities_size;
    u32 resources_size;
    u32 init_globals_len;
    u8 init_globals[init_globals_len];
    u32 grog_extern_fns_len;
    grog_extern_fn grog_extern_fns[grog_extern_fns_len];
    u32 grog_strings_len;
    grog_string grog_strings[grog_strings_len];
    u32 on_fns_len;
    on_fn on_fns[on_fns_len];
};

grog_file grog_file_at_0x00 @ 0x00;
