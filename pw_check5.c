#include "pw_check5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ncurses.h>

extern struct winsize wbuf;

void check_password_expiry(const char *username, char *output) {
   char command[256];
   snprintf(command, sizeof(command), "sudo chage -l %s", username);

   FILE *fp = popen(command, "r");
   if (fp == NULL) {
       snprintf(output, MAX_OUTPUT_LEN, "Failed to get %s user's information.\n", username);
       return;
   }

   char buffer[256];
   time_t now = time(NULL);
   struct tm last_change_tm = {0};
   int last_change_found = 0;
   char last_change_date[256] = "N/A";

   while (fgets(buffer, sizeof(buffer), fp) != NULL) {
       if (strstr(buffer, "Last password change") != NULL) {
           strncpy(last_change_date, strchr(buffer, ':') + 2, sizeof(last_change_date));
           last_change_date[strlen(last_change_date) - 1] = '\0';  // Remove newline
           if (strptime(last_change_date, "%b %d, %Y", &last_change_tm) != NULL) {
               last_change_found = 1;
           }
       }
   }


   if (last_change_found) {
       time_t last_change_time = mktime(&last_change_tm);
       double days_diff = difftime(now, last_change_time) / (60 * 60 * 24);
       snprintf(output, MAX_OUTPUT_LEN, "User: %s, Last change: %s (%d days ago)\n",
                username, last_change_date, (int)days_diff);
   } else {
       snprintf(output, MAX_OUTPUT_LEN, "User: %s, Last change: %s\n", username, "No information");
   }


   pclose(fp);
}

int get_all_users(char users[MAX_USERS][256], int *total_users) {
   FILE *fp = fopen("/etc/passwd", "r");
   if (fp == NULL) {
       perror("/etc/passwd file cannot be opened");
       return -1;
   }


   char line[256];
   int user_count = 0;
   while (fgets(line, sizeof(line), fp) != NULL && user_count < MAX_USERS) {
       char temp_line[256];
       strncpy(temp_line, line, sizeof(temp_line));
       temp_line[sizeof(temp_line) - 1] = '\0';
       char *username = strtok(temp_line, ":");
       if (username == NULL) {
           continue;
       }
       char *last_colon = strrchr(line, ':');
       if (last_colon && (strstr(last_colon, "bash") || strstr(last_colon, "sh") || strstr(last_colon, "zsh"))) {
           strncpy(users[user_count], username, 256);
           users[user_count][255] = '\0';
           user_count++;
       }
   }
   fclose(fp);
   *total_users = user_count;
   return 0;
}

void display_users_with_ncurses(const char users[MAX_USERS][256], int total_users) {
   int start_index = 0;
   char output[MAX_OUTPUT_LEN];
   initscr();
   cbreak();
   noecho();
   keypad(stdscr, TRUE);


   while (1) {
        clear();
        int rows, cols;
        (void)cols;
        getmaxyx(stdscr, rows, cols);

        int lines_per_page = rows - 4;
        int end_index = start_index + lines_per_page;
        if (end_index > total_users) {
            end_index = total_users;
        }

        mvprintw(0, 0, "Displaying users %d to %d (Total: %d)", start_index + 1, end_index, total_users);
        mvprintw(2, 0, "[Enter/Space: Next page / q: return to main]\n");
        
        for (int i = start_index; i < end_index; i++) {
            check_password_expiry(users[i], output);
            mvprintw(4 + (i - start_index), 0, "%s", output);
        }
        refresh();
        // Handle user input
        int ch = getch();
        if (ch == 'q') {
            break;
        } else if (ch == ' ' || ch == '\n' || ch == '\r' || ch == KEY_ENTER) {
            if (end_index < total_users) {
                start_index += lines_per_page;
            }
        }
    }
   endwin();
}


int pw_main(void) {
   char users[MAX_USERS][256];
   int total_users = 0;

   if (get_all_users(users, &total_users) == 0) {
       display_users_with_ncurses(users, total_users);
   } else {
       printf("Failed to get user information.\n");
   }
   return 0;
}







