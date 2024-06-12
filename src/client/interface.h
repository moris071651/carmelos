#ifndef INTERFACE_H
#define INTERFACE_H

#include "types.h"
#include "config.h"

void set_tree_items(tree_item_t* items, size_t size);

void setup_interface(void);
void login_interface(void);
void draw_interface(void);

#endif // INTERFACE_H