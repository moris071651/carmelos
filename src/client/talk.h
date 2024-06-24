#ifndef TALK_H
#define TALK_H

#include "types.h"

#include <stdbool.h>

void setup_talk(void);
void handle_communication(void);

bool talk_req_user_login(user_t* user);
bool talk_req_user_signup(user_t* user);

void talk_req_note(tree_item_t* item);
void talk_req_create_note(tree_item_t* item);
void talk_req_delete_note(tree_item_t* item);
void talk_req_save_note(editor_item_t* item);

#endif // TALK_H