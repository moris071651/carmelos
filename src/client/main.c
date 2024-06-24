#include <unistd.h>
#include <stdbool.h>

#include "talk.h"
#include "types.h"
#include "interface.h"

int main(int argc, char* arv[]) {
    

    setup_talk();
    setup_interface();

    while(true) {
        draw_interface();
        handle_communication();
        usleep(500);
    }
}
