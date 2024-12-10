#ifndef PERMISSION_CHECK_H

#include <sys/types.h>

#define PERMISSION_CHECK_H
#define MAX_LINE_LENGTH_A 512
#define MAX_LINE_LENGTH 256
#define MAX_ITEMS_PER_PAGE 10
#define LOG_FILE_PREFIX "permission_fix_log_"

typedef struct {
    char filename[MAX_LINE_LENGTH];
    char path[MAX_LINE_LENGTH];
    char old_owner[MAX_LINE_LENGTH_A];
    char new_owner[MAX_LINE_LENGTH_A];
    char old_perms[16];
    char new_perms[16];
} FileLog;

typedef struct {
    char path[MAX_LINE_LENGTH];
    char owner[MAX_LINE_LENGTH];
    char group[MAX_LINE_LENGTH];
    int perms;
} FilePermission;

void get_current_time(char *buffer, size_t size);
void truncate_string(const char *input, char *output, size_t max_length);
int parse_permissions(const char *filename, FilePermission **permissions, int *count);
int check_and_fix_permissions(FilePermission *permissions, int count, FileLog **logs, int *log_count, char *log_file);
void display_page(FileLog *logs, int total_files, int invalid_files, const char *log_file, int count, int page);
void ncurses_display(FileLog *logs, int total_files, int invalid_files, const char *log_file, int count);
int permissions_main(void);

#endif