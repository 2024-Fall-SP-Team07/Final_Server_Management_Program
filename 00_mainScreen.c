#include "00_mainScreen.h"
#include "common.h"
#include "tmp_cleanup.h"
#include "resources_monitor.h"
#include "permission_check.h"
#include "pw_check5.h"
#include "log_check_final.h"
#include "logrotate_main.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ERROR_LOG_PATH "/var/log/00_Server_Management/zz_error.log"

struct winsize wbuf;
int main(void) {
    MenuItem menu[MAX_MENU_ITEMS] = {
        {"1. Temporary File Cleaning", menu_action_1},
        {"2. Permission Checking", menu_action_2},
        {"3. Password Checking", menu_action_3},
        {"4. Resources Monitor", menu_action_4},
        {"5. Log Checking", menu_action_5},
        {"6. Log Rotate", menu_action_6},
        {"Quit", menu_action_exit}
    };
    int current = 0;
    int ch;
    
    if(geteuid() != 0) {
        printf("\nThis program must be running with root privileges. (using sudo or as root)...\nexit.\n\n");
        exit(-1);
    }

    initialization();

    while (1) {
        erase();
        display_menu(menu, current);
        ch = getch();
        switch (ch) {
            case KEY_UP:
                current = (current > 0) ? current - 1 : MAX_MENU_ITEMS - 1;
                break;
            case KEY_DOWN:
                current = (current < MAX_MENU_ITEMS - 1) ? current + 1 : 0;
                break;
            case 10:
                menu[current].action();
                break;
        }
    }
    endwin();
    return 0;
}

void initialization(void){
    int fd = -1;
    DIR* dir_ptr = NULL;
    DateInfo date = get_Date();

    close(2);
    ((dir_ptr = opendir(LOG_PATH)) == NULL) ? mkdir(LOG_PATH, (S_IRWXU | S_IRGRP | S_IXGRP) & (~S_IRWXO) ) : closedir(dir_ptr);
    if ((fd = open(ERROR_LOG_PATH, O_WRONLY | O_CREAT | O_APPEND, (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP ) & (~S_IRWXO))) == -1) {
        fd = open("/dev/tty", O_WRONLY);
        if (dup2(fd, 2) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "%04d-%02d-%02d %02d:%02d:%02d Fail to open/create: %s\n", date.year, date.month, date.day, date.hrs, date.min, date.sec, ERROR_LOG_PATH);
        return;
    }

    initscr();              
    cbreak();               
    noecho();               
    keypad(stdscr, TRUE);   
    curs_set(0);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_MAGENTA, -1);

    (ioctl(0, TIOCGWINSZ, &wbuf) == -1) ? fprintf(stderr, "%s\n", exception(-4, "main", "Windows Size", &date)) : 0;
}

void menu_action_1() {
    clear();
    echo();
    tmp_cleanup_main();
    noecho();
}

void menu_action_2() {
    clear();
    permissions_main();
}

void menu_action_3() {
    clear();
    pw_main();
}

void menu_action_4() {
    clear();
    resources_main();
}

void menu_action_5(){
    clear();
    logcheck_main();
}

void menu_action_6(){
    clear();
    logrotate_main();
}

void menu_action_exit() {
    int fd = fileno(stdin);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETOWN, 0);
    fcntl(fd, F_SETFL, flags & ~(O_ASYNC | O_NONBLOCK));
    echo();
    endwin();
    exit(0); 
}

void display_menu(MenuItem menu[], int current) {
    struct winsize wbuf;
    const char *title = "System Management Program - Main Menu";
    int title_x, title_y, start_x, start_y;
    ioctl(0, TIOCGWINSZ, &wbuf);
    title_x = (wbuf.ws_col - strlen(title)) / 2;
    title_y = (wbuf.ws_row / 2) - 9;
    start_y = (wbuf.ws_row - MAX_MENU_ITEMS * 2) / 2;
    start_x = (wbuf.ws_col - strlen("1. Temporary_File_Cleaning  ")) / 2;

    mvprintw(title_y, title_x, "%s", title);

    for (int i = 0; i < MAX_MENU_ITEMS; i++) {
        if (i == current) {
            attron(A_REVERSE);
        }
        mvprintw(start_y + 2*i, start_x, "%s", menu[i].label);
        if (i == current) {
            attroff(A_REVERSE);
        }
    }
    refresh();
}
