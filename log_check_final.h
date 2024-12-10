#ifndef LOG_CHECK_FINAL_H

#include <utmp.h>
#include <ncurses.h>

#define LOG_CHECK_FINAL_H
#define KEY_SPACE 32

typedef struct {
    struct utmp log;
    int failure_count;
} LogEntry;

int compare_logs(const void *a, const void *b);
void calculate_log_stats(const char *wtmp_file, const char *btmp_file, int *login_attempts, int *login_failures);
void print_log_info(WINDOW *win, struct utmp *log, int failure_count, int y, int x);
int read_log_file(WINDOW *win, const char *file_path, int start_line, int max_lines, int *total_lines);
void display_message(WINDOW *win, int login_attempts, int login_failures);
void display_title(WINDOW *win, int max_x);
void display_footer_message(WINDOW *win, int max_y, int max_x);
void display_more_message(WINDOW *win, int y, int current, int total);
void display_press_enter_message(WINDOW *win);
int logcheck_main();

#endif