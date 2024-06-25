#include "interface.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "talk.h"
#include "fuzzy.h"

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

#ifndef TREE_VIEW_SEARCH_PROMPT
#define TREE_VIEW_SEARCH_PROMPT "SEARCH: "
#endif

WINDOW* tree_area = NULL;
WINDOW* editor_area = NULL;
WINDOW* search_area = NULL;
WINDOW* new_note_area = NULL;

size_t tree_start = 0;
size_t tree_select = 0;

size_t tree_items_size = 0;
tree_item_t* tree_items = NULL;

size_t tree_items_size_old = 0;
tree_item_t* tree_items_old = NULL;

bool tree_need_redraw = false;
bool editor_need_redraw = false;
bool search_need_redraw = false;
bool new_note_need_redraw = false;

char search_area_buffer[257] = "";
size_t search_area_buffer_size = 0;

char new_note_area_buffer[256] = "";
size_t new_note_area_buffer_size = 0;

char** editor_buffer = NULL;
size_t editor_buffer_lines = 0;

size_t editor_start_x = 0;
size_t editor_start_y = 0;

size_t editor_cursor_x = 0;
size_t editor_cursor_y = 0;

int editor_saving = 0;

enum {
    TREE_AREA,
    EDITOR_AREA,
    SEARCH_AREA,
    NEW_NOTE_AREA,
} active_area = TREE_AREA;

user_t current_user = {0};
editor_item_t current_note = {0};

static void editor_buffer_clear(void);

static void login_screen(void) {
    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    WINDOW* username_area = newwin(3, maxx * 0.7, maxy / 2 - 4, maxx * 0.15);
    WINDOW* password_area = newwin(3, maxx * 0.7, maxy / 2 + 1, maxx * 0.15);

    user_t user;
    bool ready = false;

    do {
        clear();
        wclear(username_area);
        wclear(password_area);

        box(username_area, 0, 0);
        box(password_area, 0, 0);

        refresh();
        wrefresh(username_area);
        wrefresh(password_area);

        echo();
        mvwgetnstr(username_area, 1, 1, user.name, 127);

        noecho();
        mvwgetnstr(password_area, 1, 1, user.passwd, 127);

        mvprintw(maxy - 1, 0, "Now Press s for signup or l for login");

        int input;
        while((input = getch()) != 'l' && input != 's' && input != CTRL('q'));
        if (input == 'l') {
            if (talk_req_user_login(&user)) {
                ready = true;
            }
            else {
                mvprintw(0, 0, "wrong login");
            }
        }
        else if (input == 's') {
            if (talk_req_user_signup(&user)) {
                ready = true;
            }
            else {
                mvprintw(0, 0, "Something have gone wrong");
            }
        }

    } while (!ready);

    delwin(username_area);
    delwin(password_area);

    clear();
}

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
    
    login_screen();
    
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

static void tree_filter_items(const char* query) {
    if (tree_items_size == 0) {
        return;
    }

    if (tree_items_old) {
        free(tree_items);

        tree_items = tree_items_old;
        tree_items_size = tree_items_size_old;

        tree_items_old = NULL;
        tree_items_size_old = 0;

    }

    tree_item_t* filtered = malloc(tree_items_size * sizeof(tree_item_t));
    if (!filtered) {
        exit(EXIT_FAILURE);
    }

    size_t size = 0;
    for (size_t i = 0; i < tree_items_size; i++) {
        if (fuzzy_search(query ,tree_items[i].name)) {
            filtered[size++] = tree_items[i];
        }
    }

    filtered = realloc(filtered, size * sizeof(tree_item_t));
    if (!filtered && size != 0) {
        exit(EXIT_FAILURE);
    }

    tree_items_old = tree_items;
    tree_items_size_old = tree_items_size;

    if (size == 0) {
        free(filtered);
        filtered = NULL;
    }

    tree_items = filtered;
    tree_items_size = size;
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

static void setup_search_area(void) {
    size_t maxx = getmaxx(stdscr);
    size_t maxy = getmaxy(stdscr);

    search_area = newwin(3, maxx * 0.8, (maxy - 3) / 2, maxx * 0.1);
    box(search_area, 0, 0);
    wrefresh(search_area);
    wrefresh(stdscr);

    mvwprintw(search_area, 1, 1, "%s", TREE_VIEW_SEARCH_PROMPT);

    active_area = SEARCH_AREA;
}

static void setup_new_note_area(void) {
    size_t maxx = getmaxx(stdscr);
    size_t maxy = getmaxy(stdscr);

    new_note_area = newwin(3, maxx * 0.8, (maxy - 3) / 2, maxx * 0.1);
    box(new_note_area, 0, 0);
    wrefresh(new_note_area);
    wrefresh(stdscr);

    mvwprintw(new_note_area, 1, 1, "%s", "Name: ");

    active_area = NEW_NOTE_AREA;
}

static bool handle_tree_keys(int input) {
    if (active_area != TREE_AREA) {
        return false;
    }

    if (input == KEY_MOUSE) {
        return false;
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
        talk_req_note(&tree_items[tree_select]);
        editor_buffer_clear();
        active_area = EDITOR_AREA;
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
    else if (input == CTRL('f')) {
        setup_search_area();
    }
    else if (input == CTRL('l')
        && tree_items_old != NULL) {
        free(tree_items);

        tree_items = tree_items_old;
        tree_items_size = tree_items_size_old;

        tree_items_old = NULL;
        tree_items_size_old = 0;
    }
    else if (input == 'n') {
        setup_new_note_area();
    }
    else if (input == KEY_BACKSPACE || input == KEY_DC || input == 'd') {
        talk_req_delete_note(&tree_items[tree_select]);
    }
    else {
        tree_need_redraw = false;
        return false;
    }

    return true;
}

static bool handle_tree_mouse(int input) {
    if (active_area != TREE_AREA) {
        return false;
    }

    if (input != KEY_MOUSE) {
        return false;
    }

    MEVENT event;
    if(getmouse(&event) != OK) {
        return false;
    }

    if (getbegx(tree_area) > event.x || getmaxx(tree_area) < event.x) {
        return false;
    }

    if (getbegy(tree_area) > event.y || getmaxy(tree_area) < event.y) {
        return false;
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

        return false;
    }

    return true;
}

static void editor_buffer_clear(void) {
    if (editor_buffer == NULL) {
        return;
    }

    for (size_t i = 0; i < editor_buffer_lines; i++) {
        free(editor_buffer[i]);
    }
    
    free(editor_buffer);

    editor_buffer = NULL;
    editor_buffer_lines = 0;

    editor_cursor_x = 0;
    editor_cursor_y = 0;

    editor_start_x = 0;
    editor_start_y = 0;

    editor_need_redraw = true;
}

void editor_set_note(editor_item_t* note) {
    FILE* f = fopen("test1", "w+");

    if (!note) {
        exit(EXIT_FAILURE);
    }

    editor_buffer_clear();

    strcpy(current_note.id, note->id);
    strcpy(current_note.name, note->name);
    current_note.date = note->date;
    current_note.content = note->content;

    fprintf(f, "Note content: %p\n", note->content);

    if (note->content == NULL) {
        editor_buffer = realloc(editor_buffer, (editor_buffer_lines + 1) * sizeof(char *));
        if (!editor_buffer) {
            exit(EXIT_FAILURE);
        }

        editor_buffer[editor_buffer_lines] = strdup("");
        if (!editor_buffer[editor_buffer_lines]) {
            exit(EXIT_FAILURE);
        }

        fprintf(f, "Why am i here?\n");
        fclose(f);

        editor_buffer_lines += 1;

        return;
    }

    char* start = note->content;
    char* end = strchr(start, '\n');

    while (end != NULL) {
        size_t line_length = end - start;

        editor_buffer = realloc(editor_buffer, (editor_buffer_lines + 1) * sizeof(char*));
        if (!editor_buffer) {
            exit(EXIT_FAILURE);
        }

        editor_buffer[editor_buffer_lines] = malloc(line_length + 1);
        if (!editor_buffer[editor_buffer_lines]) {
            exit(EXIT_FAILURE);
        }

        strncpy(editor_buffer[editor_buffer_lines], start, line_length);
        editor_buffer[editor_buffer_lines][line_length] = '\0';

        editor_buffer_lines += 1;

        start = end + 1;
        end = strchr(start, '\n');
    }

    if (*start != '\0') {
        editor_buffer = realloc(editor_buffer, (editor_buffer_lines + 1) * sizeof(char *));
        if (!editor_buffer) {
            exit(EXIT_FAILURE);
        }

        editor_buffer[editor_buffer_lines] = strdup(start);
        if (!editor_buffer[editor_buffer_lines]) {
            exit(EXIT_FAILURE);
        }

        editor_buffer_lines += 1;
    } else if (end != start) {
        editor_buffer = realloc(editor_buffer, (editor_buffer_lines + 1) * sizeof(char *));
        if (!editor_buffer) {
            exit(EXIT_FAILURE);
        }

        editor_buffer[editor_buffer_lines] = strdup("");
        if (!editor_buffer[editor_buffer_lines]) {
            exit(EXIT_FAILURE);
        }

        editor_buffer_lines += 1;
    }

    fprintf(f, "Editor Buffer: %p\n", editor_buffer);
    fprintf(f, "lines: %zu\n", editor_buffer_lines);
    fprintf(f, "Should be here..\n");
    fclose(f);
}

editor_item_t editor_get_note() {
    editor_item_t note;

    strcpy(note.id, current_note.id);
    strcpy(note.name, current_note.name);
    note.date = current_note.date;

    note.content = malloc(1);
    if (note.content == NULL) {
        exit(EXIT_FAILURE);
    }
    note.content[0] = '\0';

    for (size_t i = 0; i < editor_buffer_lines; i++) {
        note.content = realloc(note.content, strlen(note.content) + strlen(editor_buffer[i]) + 1);
        if (note.content == NULL) {
            exit(EXIT_FAILURE);
        }

        strcat(note.content, editor_buffer[i]);
    }

    return note;
}

static void editor_buffer_insert_char(char ch) {
    char* line = editor_buffer[editor_cursor_y];
    size_t len = strlen(line);

    line = realloc(line, len + 2);
    if (!line) {
        exit(EXIT_FAILURE);
    }

    memmove(&line[editor_cursor_x + 1], &line[editor_cursor_x], len - editor_cursor_x + 1);

    line[editor_cursor_x] = ch;

    editor_buffer[editor_cursor_y] = line;
    editor_cursor_x++;
}

static void editor_buffer_insert_newline(void) {
    if (editor_cursor_y >= editor_buffer_lines) {
        return;
    }

    char* line = editor_buffer[editor_cursor_y];

    editor_buffer = realloc(editor_buffer, (editor_buffer_lines + 1) * sizeof(char*));
    if (!editor_buffer) {
        exit(EXIT_FAILURE);
    }

    memmove(&editor_buffer[editor_cursor_y + 1], &editor_buffer[editor_cursor_y], (editor_buffer_lines - editor_cursor_y) * sizeof(char*));

    editor_buffer[editor_cursor_y + 1] = strdup(&line[editor_cursor_x]);
    if (!editor_buffer[editor_cursor_y + 1]) {
        exit(EXIT_FAILURE);
    }

    line[editor_cursor_x] = '\0';

    editor_buffer[editor_cursor_y] = realloc(line, editor_cursor_x + 1);
    if (!editor_buffer[editor_cursor_y]) {
        exit(EXIT_FAILURE);
    }

    editor_buffer_lines++;

    editor_cursor_y++;
    editor_cursor_x = 0;
}

static void editor_buffer_delete(void) {
    char* line = editor_buffer[editor_cursor_y];
    size_t len = strlen(line);

    if (editor_cursor_x < len) {
        memmove(&line[editor_cursor_x], &line[editor_cursor_x + 1], len - editor_cursor_x);
        
        line = realloc(line, len);
        if (!line) {
            exit(EXIT_FAILURE);
        }

        editor_buffer[editor_cursor_y] = line;
    }
    else if (editor_cursor_y + 1 < editor_buffer_lines) {
        line = realloc(line, len + strlen(editor_buffer[editor_cursor_y + 1]) + 1);
        if (!line) {
            exit(EXIT_FAILURE);
        }

        strcat(line, editor_buffer[editor_cursor_y + 1]);
        free(editor_buffer[editor_cursor_y + 1]);

        memmove(&editor_buffer[editor_cursor_y + 1], &editor_buffer[editor_cursor_y + 2], (editor_buffer_lines - editor_cursor_y - 1) * sizeof(char*));
        
        editor_buffer_lines--;
        editor_buffer = realloc(editor_buffer, editor_buffer_lines * sizeof(char*));
        if (!editor_buffer) {
            exit(EXIT_FAILURE);
        }

        editor_buffer[editor_cursor_y] = line;
    }
}

static void editor_buffer_backspace(void) {
    if (editor_cursor_y >= editor_buffer_lines) {
        return;
    }

    if (editor_cursor_x > 0) {
        editor_cursor_x--;

        editor_buffer_delete();
    }
    else if (editor_cursor_y > 0) {
        editor_cursor_x = strlen(editor_buffer[editor_cursor_y - 1]);
        editor_cursor_y--;

        editor_buffer_delete();
    }
}

static bool handle_editor_keys(int input) {
    if (active_area != EDITOR_AREA) {
        return false;
    }

    if (input == KEY_MOUSE) {
        return false;
    }

    if (editor_buffer == NULL) {
        return false;
    }

    if (editor_saving == -1) {
        return false;
    }

    editor_need_redraw = true;

    if (input == KEY_UP
        && editor_cursor_y > 0) {
        editor_cursor_y--;

        size_t tmp = strlen(editor_buffer[editor_cursor_y]);
        if (editor_cursor_x > tmp) {
            editor_cursor_x = (tmp == 0) ? 0 : tmp - 1;
        }

        // TODO: editor_start_y
    }
    else if (input == KEY_DOWN
        && editor_cursor_y + 1 < editor_buffer_lines) {
        editor_cursor_y++;

        size_t tmp = strlen(editor_buffer[editor_cursor_y]);
        if (editor_cursor_x > tmp) {
            editor_cursor_x = (tmp == 0) ? 0 : tmp - 1;
        }

        // TODO: editor_start_y
    }
    else if (input == KEY_LEFT
        && editor_cursor_x > 0) {
        editor_cursor_x--;
    }
    else if (input == KEY_RIGHT
        && editor_cursor_x < strlen(editor_buffer[editor_cursor_y])) {
        editor_cursor_x++;
    }
    else if (input == KEY_HOME) {
        editor_cursor_x = 0;
    }
    else if (input == KEY_END) {
        editor_cursor_x = strlen(editor_buffer[editor_cursor_y]);
    }
    else if (input == '\n') {
        editor_buffer_insert_newline();
    }
    else if (input == KEY_DC) {
        editor_buffer_delete();
    }
    else if (input == KEY_BACKSPACE || input == 127) {
        editor_buffer_backspace();
    }
    else if (input >= 32 && input <= 126) {
        editor_buffer_insert_char(input);
    }
    else if (input == 27) {
        active_area = TREE_AREA;
        tree_need_redraw = true;
    }
    else if (input == CTRL('s')) {
        editor_item_t note = editor_get_note();
        editor_saving = -1;
        talk_req_save_note(&note);
    }
    else {
        editor_need_redraw = false;

        return false;
    }

    return true;
}

static bool handle_editor_mouse(int input) {
    if (active_area != EDITOR_AREA) {
        return false;
    }

    if (input != KEY_MOUSE) {
        return false;
    }

    MEVENT event;
    if(getmouse(&event) != OK) {
        return false;
    }

    editor_need_redraw = true;

    return true;
}

static bool handle_search_keys(int input) {
    if (search_area == NULL) {
        return false;
    }

    if (active_area != SEARCH_AREA) {
        return false;
    }

    if (input == KEY_MOUSE) {
        return false;
    }

    search_need_redraw = true;

    if (input == 10) {
        if (search_area_buffer_size > 0) {
            tree_filter_items(search_area_buffer);
        }

        wclear(tree_area);

        search_area_buffer[0] = '\0';
        search_area_buffer_size = 0;

        wclear(search_area);
        delwin(search_area);
        search_area = NULL;

        active_area = TREE_AREA;

        tree_need_redraw = true;
        editor_need_redraw = true;

        tree_select = 0;
        tree_start = 0;
    }
    else if (input == 27) {
        search_area_buffer[0] = '\0';
        search_area_buffer_size = 0;

        wclear(search_area);
        delwin(search_area);
        search_area = NULL;

        active_area = TREE_AREA;

        tree_need_redraw = true;
        editor_need_redraw = true;
    }
    else if ((input == KEY_BACKSPACE || input == KEY_DC || input == 127)
        && search_area_buffer_size != 0) {

        search_area_buffer_size--;
        search_area_buffer[search_area_buffer_size] = '\0';
    }
    else if (input >= 32 && input <= 126
        && search_area_buffer_size < 256) {
        
        search_area_buffer[search_area_buffer_size++] = input;
        search_area_buffer[search_area_buffer_size] = '\0';
    }
    else {
        search_need_redraw = false;

        return false;
    }

    return true;
}

static bool handle_new_note_keys(int input) {
    if (new_note_area == NULL) {
        return false;
    }

    if (active_area != NEW_NOTE_AREA) {
        return false;
    }

    if (input == KEY_MOUSE) {
        return false;
    }

    new_note_need_redraw = true;

    if (input == 10) {
        tree_item_t note;
        strcpy(note.name, new_note_area_buffer);
        talk_req_create_note(&note);

        new_note_area_buffer[0] = '\0';
        new_note_area_buffer_size = 0;

        wclear(new_note_area);
        delwin(new_note_area);
        new_note_area = NULL;

        active_area = TREE_AREA;

        tree_need_redraw = true;
        editor_need_redraw = true;
    }
    else if (input == 27) {
        new_note_area_buffer[0] = '\0';
        new_note_area_buffer_size = 0;

        wclear(new_note_area);
        delwin(new_note_area);
        new_note_area = NULL;

        active_area = TREE_AREA;

        tree_need_redraw = true;
        editor_need_redraw = true;
    }
    else if ((input == KEY_BACKSPACE || input == KEY_DC || input == 127)
        && new_note_area_buffer_size != 0) {

        new_note_area_buffer_size--;
        new_note_area_buffer[new_note_area_buffer_size] = '\0';
    }
    else if (input >= 32 && input <= 126
        && new_note_area_buffer_size < 255) {
        
        new_note_area_buffer[new_note_area_buffer_size++] = input;
        new_note_area_buffer[new_note_area_buffer_size] = '\0';
    }
    else {
        new_note_need_redraw = false;

        return false;
    }

    return true;
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
        if (handle_tree_keys(input)) return;
        if (handle_tree_mouse(input)) return;

        if (handle_editor_keys(input)) return;
        // if (handle_editor_mouse(input)) return;

        if (handle_search_keys(input)) return;
        if (handle_new_note_keys(input)) return;
    }
}

void tree_add_item(tree_item_t* item) {
    tree_items = realloc(tree_items, (tree_items_size + 1) * sizeof(tree_item_t));
    if (tree_items == NULL) {
        exit(EXIT_FAILURE);
    }

    tree_items[tree_items_size++] = *item;

    tree_need_redraw = true;
}

void tree_remove_item(tree_item_t* item) {
    for (size_t i = 0; i < tree_items_size; i++) {
        if (strcmp(tree_items[i].id, item->id)) {
            memmove(&tree_items[i], &tree_items[i + 1], tree_items_size - i - 1);

            tree_items = realloc(tree_items, (tree_items_size - 1) * sizeof(tree_item_t));
            if (tree_items == NULL) {
                exit(1);
            }

            tree_need_redraw = true;
        }
    }
}

static void draw_tree(void) {
    if (!tree_need_redraw) {
        return;
    }

    size_t maxx = getmaxx(tree_area);
    size_t maxy = getmaxy(tree_area);
    char buff[maxx];

    werase(tree_area);

    for (size_t i = tree_start, line = 1; i < tree_items_size; i++) {
        if (tree_select == i) {
            wattron(tree_area, A_STANDOUT);
        }

        strncpy(buff, tree_items[i].name, maxx - 2);
        buff[maxx - 1] = '\0';
        mvwprintw(tree_area, line, 1, "%s", buff);

        line++;

        wattroff(tree_area, A_STANDOUT);

        if (line > maxy) {
            break;
        }
    }

    box(tree_area, 0, 0);

    strncpy(buff, current_user.name, maxx - 4);
    mvwprintw(tree_area, 0, 1, " %s ", buff);

    wrefresh(tree_area);

    tree_need_redraw = false;
}

static void draw_editor(void) {
    if (!editor_need_redraw) {
        return;
    }

    size_t maxx = getmaxx(editor_area);
    size_t maxy = getmaxy(editor_area);
    char buff[maxx];

    werase(editor_area);

    for (size_t i = editor_start_y, line = 1; i < editor_buffer_lines; i++) {

        strncpy(buff, editor_buffer[i], maxx - 2);
        buff[maxx - 1] = '\0';
        mvwprintw(editor_area, line, 1, "%s", buff);

        if (++line > maxy) {
            break;
        }
    }

    if (active_area == EDITOR_AREA) {
        char tmp[10];
        wattron(editor_area, A_STANDOUT);
        mvwinnstr(editor_area, editor_cursor_y + 1, editor_cursor_x + 1, tmp, 1);
        mvwprintw(editor_area, editor_cursor_y + 1, editor_cursor_x + 1, "%s", tmp);
        wattroff(editor_area, A_STANDOUT);
    }

    box(editor_area, 0, 0);

    if (active_area == EDITOR_AREA && current_note.content != NULL) {
        struct tm* info = localtime(&current_note.date);
        strftime(buff, 80, "[%d/%m/%Y]", info);
        size_t width = strlen(buff) + 3;
        mvwprintw(editor_area, 0, maxx - width, " %s ", buff);

        strncpy(buff, current_note.name, maxx - width - 6);
        buff[maxx - width - 5] = '\0';
        mvwprintw(editor_area, 0, 1, " %s ", buff);
    }

    wrefresh(editor_area);

    editor_need_redraw = false;
}

static void draw_search(void) {
    if (search_area == NULL) {
        return;
    }

    if (!search_need_redraw) {
        box(search_area, 0, 0);
        wrefresh(search_area);
        return;
    }

    wclear(search_area);

    size_t width = getmaxx(search_area) - 2;
    width -= sizeof(TREE_VIEW_SEARCH_PROMPT);

    mvwprintw(search_area, 1, 1, "%s%s", TREE_VIEW_SEARCH_PROMPT, search_area_buffer + (search_area_buffer_size > width ? search_area_buffer_size - width : 0));
    
    box(search_area, 0, 0);
    wrefresh(search_area);

    search_need_redraw = false;
}

static void draw_new_note(void) {
    if (new_note_area == NULL) {
        return;
    }

    if (!new_note_need_redraw) {
        box(new_note_area, 0, 0);
        wrefresh(new_note_area);
        return;
    }

    wclear(new_note_area);

    size_t width = getmaxx(new_note_area) - 2;
    width -= sizeof("Name: ");

    mvwprintw(new_note_area, 1, 1, "%s%s", "Name: ", new_note_area_buffer + (new_note_area_buffer_size > width ? new_note_area_buffer_size - width : 0));
    
    box(new_note_area, 0, 0);
    wrefresh(new_note_area);

    new_note_need_redraw = false;
}

void draw_interface(void) {
    handle_keys();

    draw_tree();
    draw_editor();
    draw_search();
    draw_new_note();
}

void tree_set_items(tree_item_t* items, size_t size) {
    tree_items = items;
    tree_items_size = size;
}

void set_user_name(char* name) {
    strcpy(current_user.name, name);
}

void editor_change_state(int state) {
    editor_saving = state;
}

user_t* get_user(void) {
    return &current_user;
}