/*
- 2024-Fall, System Programming (ELEC0462-003)
- Professor: Jeong, Chang-Su
- Author: Seo, Hyeong-Cheol
- cpu_info.c: Related to CPU information. (Average Usage & Temperature while N minute)
- For detail explanation, Let see the header file. (cpu_info.h)
*/
#include "cpu_info_to_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

CPU_Usage get_CPU_Jiffies(DateInfo* date){
    unsigned long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0, guest = 0, guest_nice = 0;
    CPU_Usage usage;
    FILE *fp = NULL;
    ((fp = fopen("/proc/stat", "r")) == NULL) ? printf("%s\n", exception(-1, "get_CPU_Jiffies", "/proc/stat", date)) : 0;
    fscanf(fp, "cpu  %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
    fclose(fp);
    usage.totalJiff = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
    usage.idleJiff = idle;
    return usage;
}

void *write_CPU_Information(){
    int fd = -1;
    CPU_Info content;
    DIR *dir_ptr = NULL;
    ((dir_ptr = opendir(LOG_PATH)) == NULL) ? mkdir(LOG_PATH, (S_IRWXU | S_IRGRP | S_IXGRP) & (~S_IRWXO) ) : closedir(dir_ptr);
    while(1){
        content.date = get_Date();
        content.temp = get_CPU_Temperature(&(content.date));
        content.usage = get_CPU_Jiffies(&(content.date));
        ((fd = open(CPU_INFO_LOG, O_WRONLY | O_CREAT | O_APPEND, (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP ) & (~S_IRWXO))) == -1) ? printf("%s\n", exception(-1, "write_CPU_Information", "CPU Information Log", &(content.date))) : 0;
        (write(fd, &content, sizeof(CPU_Info)) != sizeof(CPU_Info)) ? printf("%s\n", exception(-3, "write_CPU_Information", "CPU Information Log", &(content.date))) : 0; 
        close(fd);
        sleep(1);
    }
    return NULL;
}

float get_CPU_Temperature(DateInfo* date){
    FILE* fp = NULL;
    float temp = 0;
    char path_type[PATH_LEN + 1] = "/sys/class/thermal/thermal_zone0/type";
    char path_temp[PATH_LEN + 1] = { '\0' };
    char type[TYPE_LEN + 2] = { '\0' };
    for (int i = 1; (fp = fopen(path_type, "r")) != NULL; i++){
        (fscanf(fp, "%s", type) == EOF) ? printf("%s\n", exception(-2, "get_CPU_Temperature", "Device Type", date)) : 0;
        if ((strcmp(type, "k10temp") == 0) || (strcmp(type, "x86_pkg_temp") == 0) || (strcmp(type, "coretemp") == 0)) {
                break;
        }
        printf("200%s\n", type);
        sprintf(path_type, "/sys/class/thermal/thermal_zone%d/type", i);
    }
    if (fp == NULL){
        return -1;
    }
    strncpy(path_temp, path_type, strlen(path_type) - 4);
    strcat(path_temp, "temp");
    fclose(fp);
    fp = NULL;
    if ((fp = fopen(path_temp, "r")) == NULL){
        printf("%s\n", exception(-1, "get_CPU_Temperature", "CPU Temperature", date));
        return 0;
    } 
    (fscanf(fp, "%f", &temp) == EOF) ? printf("%s\n", exception(-2, "get_CPU_Temperature", "CPU Temperature", date)) : 0;
    fclose(fp);
    return temp/1000;
}

