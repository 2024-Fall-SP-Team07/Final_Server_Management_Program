#include "h_for_tmp_cleanup.h"
#include "check_before_cleanup.h"
#include "cleanup.h"
#include "tmp_cleanup.h"

int ask_delete_confirmation(int* line)
{
    int ch;
    char message[128];

    clear();
    snprintf(message, sizeof(message), "Do you want to delete the files? (y/n) : ");
    mvprintw(*line, 0, "%s", message);
    refresh();
    
    while(1){
        ch = getch();
        if (ch == 'y' || ch == 'Y')
        {
            clear();
            snprintf(message, sizeof(message), "Do you want to delete the files? (y/n) : %c", ch);
            mvprintw((*line)++, 0, "%s", message);
            refresh();

            return 1;
        }
        else if (ch == 'n' || ch == 'N')
        {
            clear();
            snprintf(message, sizeof(message), "Do you want to delete the files? (y/n) : %c", ch);
            mvprintw((*line)++, 0, "%s", message);
            refresh();

            return 0;
        }

        mvprintw(2, 2, "- Invalid Input. Please Input Again.");
        refresh();
    }
    return 0;
}

int tmp_cleanup_main()
{
    initscr();
    cbreak();
    curs_set(0);
    noecho();
    
    int ch;
    int l = 0;
    int delete_files = ask_delete_confirmation(&l);
    int tmp_deleted_files_count = 0; // 삭제된 파일 개수 추적
    int var_tmp_deleted_files_count = 0;
    int var_cache_deleted_files_count = 0;
    int var_log_deleted_files_count = 0;
    int total_deleted_files = 0;

    char message[128];
    l++;
    if (delete_files)
    {
        snprintf(message, sizeof(message), "  - Deleting tmp files from /tmp...");
        mvprintw(l, 0, "%s", message);
        refresh();
        tmp_deleted_files_count = cleanup_files_recursive("/tmp", 1, tmp_deleted_files_count);
        mvprintw(l++, 45, "number of deleted file : %d", tmp_deleted_files_count);

        snprintf(message, sizeof(message), "  - Deleting tmp files from /var/tmp...");
        mvprintw(l, 0, "%s", message);
        refresh();
        var_tmp_deleted_files_count = cleanup_files_recursive("/var/tmp", 7, var_tmp_deleted_files_count);
        mvprintw(l++, 45, "number of deleted file : %d", var_tmp_deleted_files_count);
        
        snprintf(message, sizeof(message), "  - Deleting tmp files from /var/cache...");
        mvprintw(l, 0, "%s", message);
        refresh();
        var_cache_deleted_files_count = cleanup_files_recursive("/var/cache", 30, var_cache_deleted_files_count);
        mvprintw(l++, 45, "number of deleted file : %d", var_cache_deleted_files_count);

        snprintf(message, sizeof(message), "  - Deleting tmp files from /var/log...");
        mvprintw(l, 0, "%s", message);
        refresh();
        var_log_deleted_files_count = cleanup_log_files("/var/log", 365, var_log_deleted_files_count);
        mvprintw(l++, 45, "number of deleted file : %d", var_log_deleted_files_count);
        
        total_deleted_files = tmp_deleted_files_count + var_tmp_deleted_files_count + var_cache_deleted_files_count + var_log_deleted_files_count;
        snprintf(message, sizeof(message), "Deleted %d tmp files.", total_deleted_files);
        mvprintw(5 + (LINES - 5) / 2 - 1, (COLS - strlen(message)) / 2, "%s", message);
        snprintf(message, sizeof(message), "details at /var/log/00_Server_Management/deleted_tmp_files.log");
        mvprintw(5 + (LINES - 5) / 2, (COLS - strlen(message)) / 2, "%s", message);
        snprintf(message, sizeof(message), "To restore main screen, Press \"q\"");
        mvprintw(5 + (LINES - 5) / 2 + 1, (COLS - strlen(message)) / 2, "%s", message);
        refresh();
    }
    else
    {
        move(2, 2);
        addstr("- Stopped by user.");
        snprintf(message, sizeof(message), "- To restore main screen, Press \"q\".");
        mvprintw(3, 2, "%s", message);
        refresh();
    }

    while ((ch = getch()) != 'q') 
    {
        getch();
    }
    endwin();

    return 0;
}