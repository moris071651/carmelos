#ifndef INTERFACE_H
#define INTERFACE_H

#include "types.h"
#include "config.h"

void setup_interface(void);
void draw_interface(void);

void set_user_name(char* name);
user_t* get_user(void);

editor_item_t editor_get_note();
void editor_change_state(int state);
void tree_set_items(tree_item_t* items, size_t size);
void editor_set_note(editor_item_t* note);
void tree_add_item(tree_item_t* item);
void tree_remove_item(tree_item_t* item);

#endif // INTERFACE_H