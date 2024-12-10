#ifndef MAINSCREEN_H

#define MAINSCREEN_H
#define MAX_MENU_ITEMS 7

typedef struct {
    const char *label;
    void (*action)();
} MenuItem;

void signal_handling(int sig);
void restore_screen_init(void);
void initialization(void);
void signal_handling(int);
void restore_screen_init(void);
void menu_action_1();
void menu_action_2();
void menu_action_3();
void menu_action_4();
void menu_action_5();
void menu_action_6();
void menu_action_exit();
void display_menu(MenuItem menu[], int);

#endif