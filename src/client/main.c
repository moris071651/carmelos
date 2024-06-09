#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <time.h>
#include <unistd.h>

#include <ncurses.h>

void setup_display();
void draw_display();
void handle_resize();
void destroy_display();

size_t file_tree_start = 0;
size_t file_tree_select = 0;

WINDOW* file_tree_area = NULL;
WINDOW* writer_area = NULL;

enum SELECTED_AREA {
    FILE_TREE_AREA,
    WRITER_AREA,
};



typedef struct {
    size_t id;
    char* name;
    time_t date;
} FILE_TREE_ITEM;

FILE_TREE_ITEM list[10] = {
    {
        .id = 1,
        .date = 12342,
        .name = "Helsslo",
    },
        {
        .id = 2,
        .date = 1242,
        .name = "Heldfsalo",
    },
    {
        .id = 3,
        .date = 1342,
        .name = "Heloklo",
    },
    {
        .id = 4,
        .date = 4332,
        .name = "Heltyuilo",
    },
    {
        .id = 10,
        .date = 2345,
        .name = "Heedcvbllo",
    },
    {
        .id = 5,
        .date = 3383,
        .name = "Heikmnbvfghjllo",
    },
    {
        .id = 6,
        .date = 9363,
        .name = "Huwijsjsello",
    },
    {
        .id = 7,
        .date = 5388,
        .name = "Hell[983uwuso",
    },
    {
        .id = 8,
        .date = 6353,
        .name = "He623723932wwwwwwwwwwwwwwwwwwwwwskzxllo",
    },
    {
        .id = 9,
        .date = 83392,
        .name = "iiiiiilo",
    },

};


int main(int argc, char* arv[]) {
    setup_display();
    atexit(destroy_display);

    while(true) {
        draw_display();   
        usleep(500);
    }
}

void setup_display() {
    initscr();
    wclear(stdscr);

    raw();
    noecho();
    keypad(stdscr, true); 
    nodelay(stdscr, true);

    curs_set(0);
    mousemask(BUTTON1_CLICKED, NULL);

    size_t width = COLS * 0.30 > 50 ? 50 : COLS * 0.30;
    width = width < 20 ? 20 : width;

    file_tree_area = newwin(LINES, width, 0, 0);
    box(file_tree_area, 0, 0);
    wrefresh(file_tree_area);

    writer_area = newwin(LINES, COLS - width, 0, width);
    box(writer_area, 0, 0);
    wrefresh(writer_area);

    refresh();
}

void draw_display() {
    int input = getch();

    if (input == 'q') {
        exit(EXIT_SUCCESS);
    }

    if (input == KEY_RESIZE) {
        handle_resize();
    }

    if (input == KEY_UP && file_tree_select != 0) {
        file_tree_select--;

        if (file_tree_select < file_tree_start) {
            file_tree_start = file_tree_select;
        }
    }

    if (input == KEY_DOWN && file_tree_select != 199) {
        file_tree_select++;

        if (file_tree_select > file_tree_start + getmaxy(file_tree_area) - 3) {
            file_tree_start = file_tree_select - getmaxy(file_tree_area) + 3;
        }
    }

    for (size_t j = file_tree_start, line = 1; j < 200; j++) {
        int i = j % 10;
        if (file_tree_select == j) {
            wattron(file_tree_area, A_STANDOUT);
        }

        size_t width = getmaxx(file_tree_area);

        char buff[width];
        strncpy(buff, list[i].name, width - 2);
        buff[width - 1] = '\0';

        char clear_buff[width];
        memset(clear_buff, ' ', width - 2);
        clear_buff[width - 1] = '\0';

        mvwprintw(file_tree_area, line, 1, "%s", clear_buff);
        mvwprintw(file_tree_area, line++, 1, "%s", buff);

        wattroff(file_tree_area, A_STANDOUT);

        if (line > getmaxy(file_tree_area)) {
            break;
        }
    }

    box(file_tree_area, 0, 0);
    wrefresh(file_tree_area);

    box(writer_area, 0, 0);
    wrefresh(writer_area);
    
    refresh();
}

void handle_resize() {
    size_t maxx = getmaxx(stdscr);
    size_t maxy = getmaxy(stdscr);

    size_t width = maxy * 0.30 > 50 ? 50 : maxy * 0.30;
    width = width < 20 ? 20 : width;

    wclear(file_tree_area);
    mvwin(file_tree_area, 0, 0);
    wresize(file_tree_area, maxy, width);

    wclear(writer_area);
    mvwin(writer_area, 0, width);
    wresize(writer_area, maxy, maxx - width);
}

void destroy_display() {
    if (file_tree_area != NULL) {
        delwin(file_tree_area);
    }

    if (writer_area != NULL) {
        delwin(writer_area);
    }

    curs_set(1);

    endwin();
}