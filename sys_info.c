#include "common.h"
#include "sys_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/sysinfo.h>

void get_Hostname(char* buf, int size){
    FILE* fp = NULL;
    DateInfo date = get_Date();
    if ((fp = fopen("/etc/hostname", "r")) == NULL) {
        strcpy(buf, "N/A");
        fprintf(stderr, "%s\n", exception(-1, "get_Hostname", "/etc/hostname", &date));
    }
    fgets(buf, size, fp);
    fclose(fp);
    buf[strlen(buf) - 1] = '\0';
}

void get_ProductName(char* buf, int size){
    FILE *fp = NULL;
    DateInfo date = get_Date();
    if ((fp = fopen("/sys/class/dmi/id/product_name", "r")) == NULL){
        strcpy(buf, "N/A");
        fprintf(stderr, "%s\n", exception(-1, "get_ProductName", "/sys/class/dmi/id/product_name", &date));
    }
    fgets(buf, size, fp);
    fclose(fp);
    buf[strlen(buf) - 1] = '\0';
}

void get_SystemTime(char* buf){
    time_t nowTime;
    DateInfo date = get_Date();
    struct tm *lt; // Local Time
    char res[TIME_LEN] = { '\0' };
    time(&nowTime);
    lt = localtime(&nowTime);
    if (lt == NULL){
        strcpy(buf, "N/A");
        fprintf(stderr, "%s\n", exception(-4, "get_SystemTime", "SystemTime", &date));
    }
    snprintf(res, TIME_LEN, "%04d-%02d-%02d %02d:%02d:%02d", (lt->tm_year + 1900)%10000u, (lt->tm_mon + 1)%100u, (lt->tm_mday)%100u, (lt->tm_hour)%100u, (lt->tm_min)%100u, (lt->tm_sec)%100u);
    strcpy(buf, res);
}

void get_UpTime(char* buf){
    struct sysinfo info;
    char res[TIME_LEN] = { '\0' };
    DateInfo date = get_Date();
    if (sysinfo(&info) == -1) {
        strcpy(buf, "N/A");
        fprintf(stderr, "%s\n", exception(-4, "get_Uptime", "sysinfo", &date));
    }
    snprintf(res, TIME_LEN, "%d days, %02d:%02d:%02d", (int)((info.uptime / 3600) / 24)%10000u, (int)((info.uptime % (24 * 3600)) / 3600)%100u, (int)((info.uptime % 3600) / 60)%100u, (int)(info.uptime % 60)%100u);
    strcpy(buf,res);
}