#ifndef TALK_H
#define TALK_H

#include "types.h"

#include <stdbool.h>

void talk_setup(void);
void talk_handler(void);

bool talk_req_user_login(user_t* user);
bool talk_req_user_signup(user_t* user);

void talk_req_note_content(tree_item_t* item);
void talk_req_note_create(tree_item_t* item);
void talk_req_note_delete(tree_item_t* item);
void talk_req_note_save(editor_item_t* item);

#endif // TALK_H
