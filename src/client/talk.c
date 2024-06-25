#include "talk.h"
#include "types.h"
#include "config.h"

#include "interface.h"
#include "socket_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <errno.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

#ifndef TALK_SOCKET_FILE
#define TALK_SOCKET_FILE "/tmp/carmelos.sock"
#endif

int socketfd = -1;

static void destroy_talk(void) {
    close(socketfd);
}

void setup_talk(void) {
    if (access(TALK_SOCKET_FILE, F_OK) != 0) {
        fprintf(stderr, "Server Not Running\n");
        exit(EXIT_FAILURE);
    }
    
    socketfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketfd == -1) {
        fprintf(stderr, "Socket failed\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_addr;
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, TALK_SOCKET_FILE);

    if (connect(socketfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "connect failed\n");
        exit(EXIT_FAILURE);
    }

    atexit(destroy_talk);
}

static void talk_set_nonblock(void) {
    int flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);
}

static void talk_unset_nonblock(void) {
    int flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags & (~O_NONBLOCK));
}

static void talk_send_data(const AllData* data) {
    size_t size = sizeof(AllData);

    if (write(socketfd, data, size) != size) {
        exit(EXIT_FAILURE);
    }
}

static AllData talk_read_data(void) {
    AllData data;
    size_t size = sizeof(AllData);

    int status = read(socketfd, &data, size);

    if (status == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            data.type = NON_TYPE;
        }
        else {
            exit(EXIT_FAILURE);
        }
    }
    else if (status < size) {
        exit(EXIT_FAILURE);
    }

    return data;
}

bool talk_req_user_login(user_t* user) {
    AllData data;

    data.type = LOGIN_TYPE;

    strcpy(data.login.username, user->name);
    strcpy(data.login.password, user->passwd);

    talk_send_data(&data);

    data = talk_read_data();

    if (data.type == RESPONSE_TYPE && strlen(data.response.username) != 0) {
        set_user_name(data.response.username);
        talk_set_nonblock();
        return true;
    }
    else {
        return false;
    }
}

bool talk_req_user_signup(user_t* user) {
    AllData data;

    data.type = SIGNUP_TYPE;

    strcpy(data.signup.username, user->name);
    strcpy(data.signup.password, user->passwd);

    talk_send_data(&data);

    data = talk_read_data();

    if (data.type == RESPONSE_TYPE && strlen(data.response.username) != 0) {
        set_user_name(data.response.username);
        talk_set_nonblock();
        return true;
    }
    else {
        return false;
    }
}

void talk_req_note(tree_item_t* item) {
    AllData data;
    data.type = GETITEM_TYPE;

    strcpy(data.getItem.id, item->id);
    strcpy(data.getItem.filename, item->name);
    data.getItem.timestamp = item->date;

    talk_send_data(&data);
}

void talk_req_delete_note(tree_item_t* item) {
    AllData data;
    data.type = DELITEM_TYPE;

    strcpy(data.delItem.id, item->id);
    strcpy(data.delItem.filename, item->name);
    data.delItem.timestamp = item->date;

    talk_send_data(&data);
}

void talk_req_create_note(tree_item_t* item) {
    AllData data;
    data.type = NEWITEM_TYPE;

    strcpy(data.newItem.filename, item->name);
    strcpy(data.newItem.id, get_user()->name);
    data.newItem.size = 0;

    talk_send_data(&data);
}

void talk_req_save_note(editor_item_t* item) {
    AllData data;
    data.type = UPDATEITEM_TYPE;

    strcpy(data.updateItem.id, item->id);
    strcpy(data.updateItem.filename, item->name);
    data.updateItem.size = strlen(item->content);

    talk_send_data(&data);

    if (write(socketfd, item->content, data.updateItem.size) != data.updateItem.size) {
        exit(EXIT_FAILURE);
    }
}

static void talk_handle_get_item(AllData* data) {
    if (data->type != GETITEM_RESPONSE_TYPE) {
        exit(EXIT_FAILURE);
    }

    editor_item_t note;

    strcpy(note.id, data->getItem_response.id);
    strcpy(note.name, data->getItem_response.filename);
    note.date = data->getItem_response.timestamp;

    note.content = malloc(data->getItem_response.size + 1);
    if (note.content == NULL) {
        exit(EXIT_FAILURE);
    }

    // FILE* f = fopen("test6", "w");
    // fprintf(f, "%zu\n", data->getItem_response.size);
    // fflush(f);
    // fclose(f);

    if (data->getItem_response.size != 0) {
        talk_unset_nonblock();
        if (read(socketfd, note.content, data->getItem_response.size + 1) != data->getItem_response.size + 1) {
            exit(EXIT_FAILURE);
        }
        talk_set_nonblock();
    }

    note.content[data->getItem_response.size] = '\0';

    editor_set_note(&note);

    free(note.content);
}

void handle_communication(void) {
    AllData data = talk_read_data();

    tree_item_t tree_item;

    switch (data.type) {
        case NEWITEM_RESPONSE_TYPE:
            strcpy(tree_item.id, data.newItem_response.id);
            strcpy(tree_item.name, data.newItem_response.filename);            
            tree_item.date = data.newItem_response.timestamp;

            tree_add_item(&tree_item);
        break;

        case DELITEM_RESPONSE_TYPE:
            strcpy(tree_item.id, data.delItem_response.id);
            strcpy(tree_item.name, data.delItem_response.filename);            
            tree_item.date = data.delItem_response.timestamp;

            tree_remove_item(&tree_item);
        break;

        case GETITEM_RESPONSE_TYPE:
            talk_handle_get_item(&data);
        break;

        case UPDATEITEM_RESPONSE_TYPE:
            editor_change_state(0);
        break;

        case NON_TYPE:

        break;

        default:
            exit(EXIT_FAILURE);
        break;
    }
}
