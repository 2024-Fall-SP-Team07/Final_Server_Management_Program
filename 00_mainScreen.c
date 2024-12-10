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
#include <fcntl.h>

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
            case 10: // Enter
                menu[current].action(); // 선택된 메뉴 항목의 동작 실행
                break;
        }
    }
    endwin(); // ncurses 종료
    return 0;
}

void initialization(void){
    // int fd = -1, flags;
    DateInfo date = get_Date();
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
    // fd = fileno(stdin);
    // flags = fcntl(fd, F_GETFL, 0);
    // fcntl(fd, F_SETOWN, getpid());
    // fcntl(fd, F_SETFL, flags | O_ASYNC | O_NONBLOCK);
}

void menu_action_1() {
    tmp_cleanup_main();
}

void menu_action_2() {
    permissions_main();
}

void menu_action_3() {
    pw_main();
}

void menu_action_4() {
    resources_main();
}

void menu_action_5(){
    logcheck_main();
}

void menu_action_6(){
    logrotate_main();
}

void menu_action_exit() {
    int fd = fileno(stdin);
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETOWN, 0);
    fcntl(fd, F_SETFL, flags & ~(O_ASYNC | O_NONBLOCK));
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
