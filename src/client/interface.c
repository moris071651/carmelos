#include "interface.h"

#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "talk.h"
#include "types.h"

#define CTRL(x) ((x) & 0x1f)

#ifndef TREE_VIEW_MIN_WIDTH
#define TREE_VIEW_MIN_WIDTH 20
#endif

#ifndef TREE_VIEW_WIDTH_PERCENT
#define TREE_VIEW_WIDTH_PERCENT 0.3
#endif

#ifndef TREE_VIEW_MAX_WIDTH
#define TREE_VIEW_MAX_WIDTH 50
#endif

WINDOW* tree_area = NULL;
WINDOW* editor_area = NULL;

size_t tree_start = 0;
size_t tree_select = 0;

size_t tree_items_size = 0;
tree_item_t* tree_items = NULL;

bool tree_need_redraw = false;
bool editor_need_redraw = false;

enum {
    TREE_AREA,
    EDITOR_AREA,
} active_area = TREE_AREA;

static void destroy_interface(void) {
    if (tree_area != NULL) {
        delwin(tree_area);
    }

    if (editor_area != NULL) {
        delwin(editor_area);
    }

    endwin();
}

void setup_interface(void) {
    // Initialize ncurses
    initscr();
    wclear(stdscr);

    // keyboard setup
    raw();
    noecho();
    keypad(stdscr, true); 
    nodelay(stdscr, true);

    // mouse setup
    curs_set(0);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    // Find the desired tree view width
    size_t width = maxx * TREE_VIEW_WIDTH_PERCENT;
    if (width > TREE_VIEW_MAX_WIDTH) {
        width = TREE_VIEW_MAX_WIDTH;
    }
    else if (width < TREE_VIEW_MIN_WIDTH) {
        width = TREE_VIEW_MIN_WIDTH;
    }

    // Tree view of all notes
    tree_area = newwin(maxy, width, 0, 0);
    box(tree_area, 0, 0);
    wrefresh(tree_area);

    // Note editor area
    editor_area = newwin(maxy, maxx - width, 0, width);
    box(editor_area, 0, 0);
    wrefresh(editor_area);

    wrefresh(stdscr);

    tree_need_redraw = true;
    editor_need_redraw = true;

    atexit(destroy_interface);
}

int sort_alpha(const void* p1, const void* p2) {
    const tree_item_t* a = p1;
    const tree_item_t* b = p2;

    return (strcmp(a->name, b->name) > 0);
}

int sort_alpha_rev(const void* p1, const void* p2) {
    const tree_item_t* a = p1;
    const tree_item_t* b = p2;

    return (strcmp(a->name, b->name) < 0);
}

int sort_date(const void* p1, const void* p2) {
    const tree_item_t* a = p1;
    const tree_item_t* b = p2;
    
    return (a->date > b->date);
}

int sort_date_rev(const void* p1, const void* p2) {
    const tree_item_t* a = p1;
    const tree_item_t* b = p2;

    return (a->date < b->date);
}

static void handle_resize(void) {
    size_t maxx = getmaxx(stdscr);
    size_t maxy = getmaxy(stdscr);

    size_t width = maxx * TREE_VIEW_WIDTH_PERCENT;
    if (width > TREE_VIEW_MAX_WIDTH) {
        width = TREE_VIEW_MAX_WIDTH;
    }
    else if (width < TREE_VIEW_MIN_WIDTH) {
        width = TREE_VIEW_MIN_WIDTH;
    }

    wclear(tree_area);
    mvwin(tree_area, 0, 0);
    wresize(tree_area, maxy, width);

    wclear(editor_area);
    mvwin(editor_area, 0, width);
    wresize(editor_area, maxy, maxx - width);
}

static void handle_tree_keys(int input) {
    if (active_area != TREE_AREA) {
        return;
    }

    if (tree_items_size == 0) {
        return;
    }

    if (input == KEY_MOUSE) {
        return;
    }

    tree_need_redraw = true;

    if ((input == KEY_UP || input == 'k') && tree_select != 0) {
        tree_select--;

        if (tree_select < tree_start) {
            tree_start = tree_select;
        }
    }
    else if ((input == KEY_DOWN || input == 'j') && tree_select != tree_items_size - 1) {
        tree_select++;

        if (tree_select > tree_start + getmaxy(tree_area) - 3) {
            tree_start = tree_select - getmaxy(tree_area) + 3;
        }
    }
    else if (input == '\n' || input == 'l') {
        // TODO: How to open notes
    }
    else if (input == KEY_F(1)) {
        qsort(tree_items, tree_items_size, sizeof(tree_item_t), sort_alpha);
    }
    else if (input == KEY_F(2)) {
        qsort(tree_items, tree_items_size, sizeof(tree_item_t), sort_alpha_rev);
    }
    else if (input == KEY_F(3)) {
        qsort(tree_items, tree_items_size, sizeof(tree_item_t), sort_date);
    }
    else if (input == KEY_F(4)) {
        qsort(tree_items, tree_items_size, sizeof(tree_item_t), sort_date_rev);
    }
    else {
        tree_need_redraw = false;
    }
}

static void handle_tree_mouse(int input) {
    if (active_area != TREE_AREA) {
        return;
    }

    if (input != KEY_MOUSE) {
        return;
    }

    MEVENT event;
    if(getmouse(&event) != OK) {
        return;
    }

    tree_need_redraw = true;

    if (event.bstate & BUTTON4_PRESSED && tree_select != 0) {
        tree_select--;

        if (tree_select < tree_start) {
            tree_start = tree_select;
        }
	}
    else if (event.bstate & BUTTON5_PRESSED && tree_select != tree_items_size - 1) {
        tree_select++;

        if (tree_select > tree_start + getmaxy(tree_area) - 3) {
            tree_start = tree_select - getmaxy(tree_area) + 3;
        }
    }
    else if (event.bstate & BUTTON1_CLICKED
        && tree_start + event.y <= tree_items_size
        && tree_start + event.y != 0) {

        tree_select = tree_start + event.y - 1;
    }
    else if (event.bstate & BUTTON1_DOUBLE_CLICKED
        && tree_start + event.y <= tree_items_size
        && tree_start + event.y != 0) {

        tree_select = tree_start + event.y - 1;

        // TODO: How to open notes
    }
    else {
        tree_need_redraw = false;
    }
}

static void handle_editor_keys(int input) {
    if (active_area != EDITOR_AREA) {
        return;
    }

    if (input == KEY_MOUSE) {
        return;
    }

    editor_need_redraw = true;
}

static void handle_editor_mouse(int input) {
    if (active_area != EDITOR_AREA) {
        return;
    }

    if (input != KEY_MOUSE) {
        return;
    }

    MEVENT event;
    if(getmouse(&event) != OK) {
        return;
    }

    editor_need_redraw = true;
}

static void handle_keys() {
    int input = getch();

    if (input == ERR) {
        return;
    }

    if (input == CTRL('q')) {
        exit(EXIT_SUCCESS);
    }
    else if (input == KEY_RESIZE) {
        handle_resize();

        tree_need_redraw = true;
        editor_need_redraw = true;
    }
    else {
        handle_tree_keys(input);
        handle_tree_mouse(input);

        handle_editor_keys(input);
        handle_editor_mouse(input);
    }
}


static void draw_tree(void) {
    if (!tree_need_redraw) {
        box(tree_area, 0, 0);
        wrefresh(tree_area);
        return;
    }

    box(tree_area, 0, 0);
    wrefresh(tree_area);

    for (size_t i = tree_start, line = 1; i < tree_items_size; i++) {
        if (tree_select == i) {
            wattron(tree_area, A_STANDOUT);
        }

        size_t maxx = getmaxx(tree_area);
        char buff[maxx];

        memset(buff, ' ', maxx - 2);
        buff[maxx - 1] = '\0';
        mvwprintw(tree_area, line, 1, "%s", buff);

        strncpy(buff, tree_items[i].name, maxx - 2);
        buff[maxx - 1] = '\0';
        mvwprintw(tree_area, line, 1, "%s", buff);

        line++;

        wattroff(tree_area, A_STANDOUT);

        if (line > getmaxy(tree_area)) {
            break;
        }
    }

    tree_need_redraw = false;
}

static void draw_writer(void) {
    if (!editor_need_redraw) {
        box(editor_area, 0, 0);
        wrefresh(editor_area);
        return;
    }

    box(editor_area, 0, 0);
    wrefresh(editor_area);

    editor_need_redraw = false;
}

void draw_interface(void) {
    handle_keys();

    draw_tree();
    draw_writer();
}

void set_tree_items(tree_item_t* items, size_t size) {
    tree_items = items;
    tree_items_size = size;
}