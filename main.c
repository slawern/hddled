/***
 * 
 *  Copyright © 2022 Sławek Wernikowski (slawern)
 *  hddled -- emulate HDD LED using status bar icon
 *
 */

#define COPYRIGHT_STRING            "Copyright © 2022 Sławek Wernikowski (slawern)"
#define MIT_LICENCE_STRING          "Permission is hereby granted, free of charge, to any person obtaining\n"           \
                                    "a copy of this software and associated documentation files (the \"Software\"),\n"  \
                                    "to deal in the Software without restriction, including without limitation\n"       \
                                    "the rights to use, copy, modify, merge, publish, distribute, sublicense,\n"        \
                                    "and/or sell copies of the Software, and to permit persons to whom the Software\n"  \
                                    "is furnished to do so, subject to the following conditions:\n"                     \
                                    "\n"                                                                                \
                                    "The above copyright notice and this permission notice shall be included in all\n"  \
                                    "copies or substantial portions of the Software.\n"                                 \
                                    "\n"                                                                                \
                                    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"    \
                                    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"        \
                                    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL\n"         \
                                    "HE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"       \
                                    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"   \
                                    "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n"       \
                                    "THE SOFTWARE.\n"

#define _GNU_SOURCE
#include <stdio.h>
#undef  _GNU_SOURCE
#include <stdbool.h>
#include <stdnoreturn.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/inotify.h>
#include <gtk/gtk.h>

#include "icon.h"

#define VERSION_STRING              "hddled 0.35/20220708"
#define DEFAULT_UPDATE_INTERVAL     100     // ms->0.1s
#define MIN_UPDATE_INTERVAL         10
#define MAX_UPDATE_INTERVAL         10000
#define NEW_DEV_UPDATE_INTERVAL     1000    // ms->1s
#define SPINLOCK_INTERVAL           100     // us->0.1s
#define PID_FILE_NAME               ".hddled.pid"
#define WR_MASK                     0x01
#define RD_MASK                     0x02
#define WRITE_LED_COLOR             "0000FF"
#define READ_LED_COLOR              "00FF00"
#define INACTIVE_LED_COLOR          "3C3C3C"
#define BACKGROUND_COLOR            "000000"

typedef long long int LONGLONG;

typedef struct {
    char            blk_device[NAME_MAX + 1],
                    blk_device_stat[PATH_MAX + 1],
                    status;
    LONGLONG        read, 
                    prev_read;
    LONGLONG        written, 
                    prev_written;
    GtkStatusIcon  *status_icon;
    GdkPixbuf      *current_icon, 
                   *last_icon;
} BLK_DEV;

typedef struct {
    bool            empty;
    char            name[NAME_MAX + 1];
} NEW_DEV_STORAGE;

GdkPixbuf           *icon_xx, 
                    *icon_Rx, 
                    *icon_xW, 
                    *icon_RW;
char                *ICON_PATTERN_xx[ICON_PATTERN_ELEMS],
                    *ICON_PATTERN_Rx[ICON_PATTERN_ELEMS],
                    *ICON_PATTERN_xW[ICON_PATTERN_ELEMS],
                    *ICON_PATTERN_RW[ICON_PATTERN_ELEMS];
char                *my_name;
NEW_DEV_STORAGE      new_dev_storage        = { true };
unsigned int         update_interval        = DEFAULT_UPDATE_INTERVAL;
bool                 opt_kill               = false,
                     opt_daemon             = false,
                     opt_help               = false,
                     opt_licence            = false;
char                *opt_interval           = NULL,
                    *opt_read_color         = NULL,
                    *opt_write_color        = NULL,
                    *opt_background_color   = NULL,
                    *opt_inactive_color     = NULL;

void usage_message(void) {
    fprintf(stderr,
            "Usage: %s [options...]\n"
            "\nPossible options are:"
            "\n -b RRGGBB       - LEDs' background color (defaults to " BACKGROUND_COLOR ")"
            "\n -d              - daemonize hddled process"
            "\n -h              - print this help message and quit"
            "\n -i RRGGBB       - inactive LED color (defaults to " INACTIVE_LED_COLOR ")"
            "\n -l              - print license notice and quit"
            "\n -r RRGGBB       - disk read LED color (defaults to " READ_LED_COLOR ")"
            "\n -u milliseconds - interval between subsequent disk activity checks (defaults to %d)"
            "\n -q              - quit the previously started daemon"
            "\n -w RRGGBB       - disk write LED color (defaults to " WRITE_LED_COLOR ")"
            "\n\n",
            my_name, DEFAULT_UPDATE_INTERVAL);
}

void error(const char *format, ...) {
    char        *message;
    va_list     list;

    va_start(list, format);
    vasprintf(&message, format, list);
    fprintf(stderr, "%s: %s\n\n", my_name, message);
    va_end(list);
    free(message);
    exit(EXIT_FAILURE);
}

char *base_name(char *path) {
    char *here = strrchr(path, '/');
    return here ? here + 1 : path;
}

char *mk_pid_file_name(char *name_buff) {
    sprintf(name_buff, "%s/%s", getenv("HOME"), PID_FILE_NAME);
    return name_buff;
}

bool is_pid_file_valid(pid_t pid) {
    char name[PATH_MAX + 1];

    sprintf(name, "/proc/%d/comm", pid);
    int fd = open(name, O_RDONLY);
    if(fd < 0)
        return false;
    ssize_t len = read(fd, name, PATH_MAX);
    close(fd);
    name[len - 1] = '\0';
    return strcmp(name, my_name) == 0;
}


bool write_pid_file(char *filename) {
    char buff[16];

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(fd < 0)
        return false;
    int len = sprintf(buff, "%d", getpid());
    write(fd, buff, len);
    close(fd);
    return true;
}

pid_t read_pid_file(char *filename) {
    char        buff[1024];
    pid_t       pid = 0;

    int fd = open(filename, O_RDONLY);
    if(-1 == fd)
        return -1;
    ssize_t len = read(fd, buff, sizeof(buff) - 1);
    buff[len] = '\0';
    if(len >= 1)
        pid = (pid_t) strtoll(buff, NULL, 10);
    close(fd);
    return pid;
}

 bool is_color_ok(const char *opt_str) {
    if(strlen(opt_str) != 6)
        return false;
    for(int i = 0; i < 6; i++)
        if(!isxdigit(opt_str[i]))
            return false;
    return true;
}

bool set_color(char **icon, int index, const char *color) {

    if(!is_color_ok(color))
        return false;

    char code = '\0';

    switch(index)
    {
        case BACKGROUND_COLOR_IX:
            code = ' ';
            break;
        case LOWER_ARROW_IX:
            code = 'W';
            break;
        case UPPER_ARROW_IX:
            code = 'R';
            break;
        default:
            ;
    }
    assert(code != '\0');
    if(icon[index] != NULL)
        free(icon[index]);
    asprintf(icon + index, "%c c #%s", code, color);
    return true;
}

void process_options(int argc, char *argv[]) {
    int     option;
    int     opt_counter = 0;
    char   *end_ptr;

    my_name = argv[0] = base_name(argv[0]);
    opterr = 0;
    while ((option = getopt(argc, argv, "b:dhi:lr:u:w:q")) != -1) {
        switch (option) {
            case 'b':
                opt_counter++;
                opt_background_color = optarg;
                break;
            case 'd':
                opt_counter++;
                opt_daemon = true;
                break;
            case 'h':
                opt_counter++;
                opt_help = true;
                break;
            case 'i':
                opt_counter++;
                opt_inactive_color = optarg;
                break;
            case 'l':
                opt_counter++;
                opt_licence = true;
                break;
            case 'r':
                opt_counter++;
                opt_read_color = optarg;
                break;
            case 'q':
                opt_counter++;
                opt_kill = true;
                break;
            case 'u':
                opt_counter++;
                opt_interval = optarg;
                break;
            case 'w':
                opt_counter++;
                opt_write_color = optarg;
                break;
            default: /* '?' */
                error("unknown option: %s", argv[optind - 1]);
        }
    }
    if (optind < argc)
       error("unexpected argument: %s", argv[optind]);
    if(opt_kill) {
        if(opt_counter > 1)
            error("-q must be used as the only option");
        char filename[PATH_MAX + 1];
        mk_pid_file_name(filename);
        pid_t pid = read_pid_file(filename);
        if(pid < 0)
            error("pid file not found");
        else {
            int result;
            if(is_pid_file_valid(pid))
                result = kill(pid, SIGKILL);
            else {
                error("no valid pid file found");
                result = 1;
            }
            unlink(filename);
            exit(0 == result ? EXIT_SUCCESS : EXIT_FAILURE);
        }
    }
    if(opt_interval) {
        LONGLONG  m_seconds = strtoll(opt_interval, &end_ptr, 10);
        if(*end_ptr != '\0' || m_seconds < MIN_UPDATE_INTERVAL || m_seconds > MAX_UPDATE_INTERVAL)
            error("illegal update interval value: %s", opt_interval);
        update_interval = m_seconds;
    }
    if(opt_read_color) {
        if(!is_color_ok(opt_read_color))
            error("bad read icon color specification -- 6 hex digits expected");
        set_color(ICON_PATTERN_Rx, UPPER_ARROW_IX, opt_read_color);
        set_color(ICON_PATTERN_RW, UPPER_ARROW_IX, opt_read_color);
    }
    if(opt_write_color) {
        if(!is_color_ok(opt_write_color))
            error("bad write icon color specification -- 6 hex digits expected");
        set_color(ICON_PATTERN_xW, LOWER_ARROW_IX, opt_write_color);
        set_color(ICON_PATTERN_RW, LOWER_ARROW_IX, opt_write_color);
    }
    if(opt_background_color) {
        if(!is_color_ok(opt_background_color))
            error("bad background color specification -- 6 hex digits expected");
        set_color(ICON_PATTERN_xx, BACKGROUND_COLOR_IX, opt_background_color);
        set_color(ICON_PATTERN_Rx, BACKGROUND_COLOR_IX, opt_background_color);
        set_color(ICON_PATTERN_xW, BACKGROUND_COLOR_IX, opt_background_color);
        set_color(ICON_PATTERN_RW, BACKGROUND_COLOR_IX, opt_background_color);
    }
    if(opt_inactive_color) {
        if(!is_color_ok(opt_inactive_color))
            error("bad inactive color specification -- 6 hex digits expected");
        set_color(ICON_PATTERN_xx, UPPER_ARROW_IX, opt_inactive_color);
        set_color(ICON_PATTERN_xx, LOWER_ARROW_IX, opt_inactive_color);
        set_color(ICON_PATTERN_xW, UPPER_ARROW_IX, opt_inactive_color);
        set_color(ICON_PATTERN_Rx, LOWER_ARROW_IX, opt_inactive_color);
    }
    if(opt_daemon) {
        char    name[PATH_MAX + 1];
        int     result;

        mk_pid_file_name(name);
        pid_t pid = read_pid_file(name);
        if(pid > 0) {
            if(is_pid_file_valid(pid))
                error("valid pid file found - is %s running?", my_name);
            result = unlink(name);
            assert(result == 0);
        }
        result = daemon(false, false);
        assert(result == 0);
        if(!write_pid_file(name))
            error("cannot write pid file");
    }
}

char *skip_blanks(char *ptr) {
    while(isspace(*ptr))
        ptr++;
    return ptr;
}

char *skip_non_blanks(char *ptr) {
    while(!isspace(*ptr))
        ptr++;
    return ptr;
}

bool read_stat(BLK_DEV *stat_data) {
    char buff[4096], *end_ptr;

    int fd = open(stat_data->blk_device_stat, O_RDONLY);
    if(fd < 0)
        return false;
    ssize_t length = read(fd, buff, sizeof(buff));
    assert(length > 100);
    close(fd);

    char *p1 = skip_blanks(buff), *p2 = skip_non_blanks(p1);
    *p2 = '\0';
    stat_data->status = 0;
    stat_data->prev_read = stat_data->read;
    stat_data->read = strtoll(p1, &end_ptr, 10);
    if(stat_data->prev_read != stat_data->read)
        stat_data->status |= RD_MASK;
    p2++;
    for(int i = 0; i < 4; i++) {
        p1 = skip_blanks(p2);
        p2 = skip_non_blanks(p1);
    }
    *p2 = '\0';
    stat_data->prev_written = stat_data->written;
    stat_data->written = strtoll(p1, &end_ptr, 10);
    if(stat_data->prev_written != stat_data->written)
        stat_data->status |= WR_MASK;
    return true;
}

void rm_blk_dev(BLK_DEV *dev) {
    gtk_status_icon_set_visible(dev->status_icon, false);
    g_object_unref(G_OBJECT(dev->status_icon));
    free(dev);
}

gboolean update(gpointer _dev) {
    bool        result;
    BLK_DEV    *dev     = (BLK_DEV *)_dev;
    bool        exists  = read_stat(dev);

    if(!exists) {
        rm_blk_dev(dev);
        result = false;
    }
    else {
        switch (dev->status) {
            case 0:     // no changes
                dev->current_icon = icon_xx;
                break;
            case 1:     // written changed
                dev->current_icon = icon_xW;
                break;
            case 2:     // read changed
                dev->current_icon = icon_Rx;
                break;
            case 3:     // read and written changed
                dev->current_icon = icon_RW;
                break;
            default:    // ?
                ;
        }
        if (dev->current_icon != dev->last_icon) {
            gtk_status_icon_set_from_pixbuf(dev->status_icon, dev->current_icon);
            gtk_status_icon_set_visible(dev->status_icon, true);
            dev->last_icon = dev->current_icon;
        }
        result = true;
    }
    return result;
}

char *mk_blk_dev_stat_name(char *stat_name, const char *dev_name) {
    sprintf(stat_name, "/sys/block/%s/stat", dev_name);
    return stat_name;
}

BLK_DEV *mk_blk_dev(char *name) {
    BLK_DEV *dev = malloc(sizeof(BLK_DEV));
    assert(dev != NULL);
    strcpy(dev->blk_device, name);
    mk_blk_dev_stat_name(dev->blk_device_stat, name);
    dev->status_icon = gtk_status_icon_new();
    gtk_status_icon_set_from_pixbuf(dev->status_icon, icon_xx);
    gtk_status_icon_set_visible(dev->status_icon, true);
    gtk_status_icon_set_tooltip_text(dev->status_icon, dev->blk_device);
    dev->last_icon = dev->current_icon = icon_xx;
    dev->prev_read = dev->prev_written = 0;
    return dev;
}

bool filter_name(const char *name) {
    static char *excluded[] = { ".", "loop", NULL };
    for(char **p = excluded; *p != NULL; p++)
        if(0 == strncmp(name, *p, strlen(*p)))
            return false;
    return true;
}

int filter(const struct dirent *entry) {
    return filter_name(entry->d_name);
}

int scan_devices(void) {
    struct dirent **namelist;

    int count = scandir("/sys/block", &namelist, filter, NULL);
    if(count >= 0) {
        for(int i = 0; i < count; i++) {
            BLK_DEV *new_dev = mk_blk_dev(namelist[i]->d_name);
            read_stat(new_dev);
            g_timeout_add(update_interval, update, new_dev);
            free(namelist[i]);
        }
        free(namelist);
    }
    return count;
}

noreturn void *new_dev_monitor() {
    int fd = inotify_init();
    assert(fd >= 0);
    int wd = inotify_add_watch(fd, "/dev", IN_CREATE);
    assert(wd >= 0);
    for(;;) {
        char _event[sizeof(struct inotify_event) + NAME_MAX + 1];
        ssize_t res = read(fd, _event, sizeof(_event));
        assert(res > 0);
        struct inotify_event *event = (struct inotify_event *) _event;
        if(event->len > 0            && 
           event->mask & IN_CREATE   && 
           !(event->mask & IN_ISDIR) && 
           filter_name(event->name)) {
                char dev_name[NAME_MAX + 1];
                mk_blk_dev_stat_name(dev_name, event->name);
                if(0 == access(dev_name, R_OK)) {
                    while(!new_dev_storage.empty)
                        usleep(SPINLOCK_INTERVAL);
                    strcpy(new_dev_storage.name, event->name);
                    new_dev_storage.empty = false;
            }
        }
    }
}

gboolean register_new_dev() {
    if(!new_dev_storage.empty) {
        BLK_DEV *new_dev = mk_blk_dev(new_dev_storage.name);
        read_stat(new_dev);
        g_timeout_add(update_interval, update, new_dev);
        new_dev_storage.empty = true;
    }
    return true;
}

void handler() {
    char file_name[PATH_MAX + 1];
    mk_pid_file_name(file_name);
    unlink(file_name);
}

void mk_icons(void) {
    int size = sizeof(ICON_PATTERN);

    memcpy(ICON_PATTERN_xx, ICON_PATTERN, size);
    memcpy(ICON_PATTERN_Rx, ICON_PATTERN, size);
    memcpy(ICON_PATTERN_xW, ICON_PATTERN, size);
    memcpy(ICON_PATTERN_RW, ICON_PATTERN, size);

    set_color(ICON_PATTERN_xx, UPPER_ARROW_IX,      INACTIVE_LED_COLOR);
    set_color(ICON_PATTERN_xx, LOWER_ARROW_IX,      INACTIVE_LED_COLOR);
    set_color(ICON_PATTERN_xx, BACKGROUND_COLOR_IX, BACKGROUND_COLOR);

    set_color(ICON_PATTERN_Rx, UPPER_ARROW_IX,      READ_LED_COLOR);
    set_color(ICON_PATTERN_Rx, LOWER_ARROW_IX,      INACTIVE_LED_COLOR);
    set_color(ICON_PATTERN_Rx, BACKGROUND_COLOR_IX, BACKGROUND_COLOR);

    set_color(ICON_PATTERN_xW, UPPER_ARROW_IX,      INACTIVE_LED_COLOR);
    set_color(ICON_PATTERN_xW, LOWER_ARROW_IX,      WRITE_LED_COLOR);
    set_color(ICON_PATTERN_xW, BACKGROUND_COLOR_IX, BACKGROUND_COLOR);

    set_color(ICON_PATTERN_RW, UPPER_ARROW_IX,      READ_LED_COLOR);
    set_color(ICON_PATTERN_RW, LOWER_ARROW_IX,      WRITE_LED_COLOR);
    set_color(ICON_PATTERN_RW, BACKGROUND_COLOR_IX, BACKGROUND_COLOR);
}

int main(int argc, char *argv[]) {
    fprintf(stderr, "\n" VERSION_STRING " " COPYRIGHT_STRING "\n\n");
    mk_icons();
    process_options(argc, argv);
    if(opt_help)
        usage_message();
    if(opt_licence)
        puts("\n" MIT_LICENCE_STRING);
    if(opt_help || opt_licence)
        return EXIT_SUCCESS;
    fprintf(stderr, "Run '%s -h' to see possible options\n", my_name);
    gdk_threads_init();
    gtk_init(NULL, NULL);
    icon_xx = gdk_pixbuf_new_from_xpm_data((const char **) ICON_PATTERN_xx);
    icon_Rx = gdk_pixbuf_new_from_xpm_data((const char **) ICON_PATTERN_Rx);
    icon_xW = gdk_pixbuf_new_from_xpm_data((const char **) ICON_PATTERN_xW);
    icon_RW = gdk_pixbuf_new_from_xpm_data((const char **) ICON_PATTERN_RW);
    int dev_no = scan_devices();
    assert(dev_no > 0);
    g_timeout_add(NEW_DEV_UPDATE_INTERVAL, register_new_dev, NULL);

    GThread *monitor = g_thread_try_new("monitor", new_dev_monitor, NULL, NULL);
    assert(monitor != NULL);
    if(opt_daemon) {
        signal(SIGTERM, handler);
        signal(SIGINT, handler);
    }
    else
        fprintf(stderr,"\n%s is running - press Ctrl-C to stop...\n", my_name);
    gtk_main();
    return EXIT_SUCCESS;
}
