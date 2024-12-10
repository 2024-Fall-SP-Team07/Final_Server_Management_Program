#ifndef PW_CHECK5_H

#define PW_CHECK5_H
#define MAX_USERS 100
#define MAX_OUTPUT_LEN 512
#define _XOPEN_SOURCE 700

void check_password_expiry(const char *username, char *output);
int get_all_users(char users[MAX_USERS][256], int *total_users);
void display_users_with_ncurses(const char users[MAX_USERS][256], int total_users);
int pw_main(void);
#endif