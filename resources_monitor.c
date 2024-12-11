#include "resources_display.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ncurses.h>

#define ERROR_LOG_PATH_MONITOR "/var/log/00_Server_Management/zz_resources_monitor_error.log"
int resources_main(void){
    resources_initialization();
    display_main();
    return 0;
}