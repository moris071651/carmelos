#include <unistd.h>
#include <stdbool.h>

#include "talk.h"
#include "interface.h"

tree_item_t file_tree_list[] = {
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
    // {
    //     .id = 3,
    //     .date = 1342,
    //     .name = "Heloklo",
    // },
    // {
    //     .id = 4,
    //     .date = 4332,
    //     .name = "Heltyuilo",
    // },
    // {
    //     .id = 10,
    //     .date = 2345,
    //     .name = "Heedcvbllo",
    // },
    // {
    //     .id = 5,
    //     .date = 3383,
    //     .name = "Heikmnbvfghjllo",
    // },
    // {
    //     .id = 6,
    //     .date = 9363,
    //     .name = "Huwijsjsello",
    // },
    // {
    //     .id = 7,
    //     .date = 5388,
    //     .name = "Hell[983uwuso",
    // },
    // {
    //     .id = 8,
    //     .date = 6353,
    //     .name = "He623723932wwwwwwwwwwwwwwwwwwwwwskzxllo",
    // },
    // {
    //     .id = 9,
    //     .date = 83392,
    //     .name = "iiiiiilo",
    // },
    // {
    //     .id = 1,
    //     .date = 12342,
    //     .name = "Helsslo",
    // },
    //     {
    //     .id = 2,
    //     .date = 1242,
    //     .name = "Heldfsalo",
    // },
    // {
    //     .id = 3,
    //     .date = 1342,
    //     .name = "Heloklo",
    // },
    // {
    //     .id = 4,
    //     .date = 4332,
    //     .name = "Heltyuilo",
    // },
    // {
    //     .id = 10,
    //     .date = 2345,
    //     .name = "Heedcvbllo",
    // },
    // {
    //     .id = 5,
    //     .date = 3383,
    //     .name = "Heikmnbvfghjllo",
    // },
    // {
    //     .id = 6,
    //     .date = 9363,
    //     .name = "Huwijsjsello",
    // },
    // {
    //     .id = 7,
    //     .date = 5388,
    //     .name = "Hell[983uwuso",
    // },
    // {
    //     .id = 8,
    //     .date = 6353,
    //     .name = "He623723932wwwwwwwwwwwwwwwwwwwwwskzxllo",
    // },
    // {
    //     .id = 9,
    //     .date = 83392,
    //     .name = "iiiiiilo",
    // },
    // {
    //     .id = 1,
    //     .date = 12342,
    //     .name = "Helsslo",
    // },
    //     {
    //     .id = 2,
    //     .date = 1242,
    //     .name = "Heldfsalo",
    // },
    // {
    //     .id = 3,
    //     .date = 1342,
    //     .name = "Heloklo",
    // },
    // {
    //     .id = 4,
    //     .date = 4332,
    //     .name = "Heltyuilo",
    // },
    // {
    //     .id = 10,
    //     .date = 2345,
    //     .name = "Heedcvbllo",
    // },
    // {
    //     .id = 5,
    //     .date = 3383,
    //     .name = "Heikmnbvfghjllo",
    // },
    // {
    //     .id = 6,
    //     .date = 9363,
    //     .name = "Huwijsjsello",
    // },
    // {
    //     .id = 7,
    //     .date = 5388,
    //     .name = "Hell[983uwuso",
    // },
    // {
    //     .id = 8,
    //     .date = 6353,
    //     .name = "He623723932wwwwwwwwwwwwwwwwwwwwwskzxllo",
    // },
    // {
    //     .id = 9,
    //     .date = 83392,
    //     .name = "iiiiiilo",
    // },
    // {
    //     .id = 1,
    //     .date = 12342,
    //     .name = "Helsslo",
    // },
    //     {
    //     .id = 2,
    //     .date = 1242,
    //     .name = "Heldfsalo",
    // },
    // {
    //     .id = 3,
    //     .date = 1342,
    //     .name = "Heloklo",
    // },
    // {
    //     .id = 4,
    //     .date = 4332,
    //     .name = "Heltyuilo",
    // },
    // {
    //     .id = 10,
    //     .date = 2345,
    //     .name = "Heedcvbllo",
    // },
    // {
    //     .id = 5,
    //     .date = 3383,
    //     .name = "Heikmnbvfghjllo",
    // },
    // {
    //     .id = 6,
    //     .date = 9363,
    //     .name = "Huwijsjsello",
    // },
    // {
    //     .id = 7,
    //     .date = 5388,
    //     .name = "Hell[983uwuso",
    // },
    // {
    //     .id = 8,
    //     .date = 6353,
    //     .name = "He623723932wwwwwwwwwwwwwwwwwwwwwskzxllo",
    // },
    // {
    //     .id = 9,
    //     .date = 83392,
    //     .name = "iiiiiilo",
    // },
    // {
    //     .id = 1,
    //     .date = 12342,
    //     .name = "Helsslo",
    // },
    //     {
    //     .id = 2,
    //     .date = 1242,
    //     .name = "Heldfsalo",
    // },
    // {
    //     .id = 3,
    //     .date = 1342,
    //     .name = "Heloklo",
    // },
    // {
    //     .id = 4,
    //     .date = 4332,
    //     .name = "Heltyuilo",
    // },
    // {
    //     .id = 10,
    //     .date = 2345,
    //     .name = "Heedcvbllo",
    // },
    // {
    //     .id = 5,
    //     .date = 3383,
    //     .name = "Heikmnbvfghjllo",
    // },
    // {
    //     .id = 6,
    //     .date = 9363,
    //     .name = "Huwijsjsello",
    // },
    // {
    //     .id = 7,
    //     .date = 5388,
    //     .name = "Hell[983uwuso",
    // },
    // {
    //     .id = 8,
    //     .date = 6353,
    //     .name = "He623723932wwwwwwwwwwwwwwwwwwwwwskzxllo",
    // },
    // {
    //     .id = 9,
    //     .date = 83392,
    //     .name = "iiiiiilo",
    // },
    // {
    //     .id = 1,
    //     .date = 12342,
    //     .name = "Helsslo",
    // },
    //     {
    //     .id = 2,
    //     .date = 1242,
    //     .name = "Heldfsalo",
    // },
    // {
    //     .id = 3,
    //     .date = 1342,
    //     .name = "Heloklo",
    // },
    // {
    //     .id = 4,
    //     .date = 4332,
    //     .name = "Heltyuilo",
    // },
    // {
    //     .id = 10,
    //     .date = 2345,
    //     .name = "Heedcvbllo",
    // },
    // {
    //     .id = 5,
    //     .date = 3383,
    //     .name = "Heikmnbvfghjllo",
    // },
    // {
    //     .id = 6,
    //     .date = 9363,
    //     .name = "Huwijsjsello",
    // },
    // {
    //     .id = 7,
    //     .date = 5388,
    //     .name = "Hell[983uwuso",
    // },
    // {
    //     .id = 8,
    //     .date = 6353,
    //     .name = "He623723932wwwwwwwwwwwwwwwwwwwwwskzxllo",
    // },
    // {
    //     .id = 9,
    //     .date = 83392,
    //     .name = "iiiiiilo",
    // },
};

#include <string.h>


int main(int argc, char* arv[]) {
    set_tree_items(file_tree_list, 52);

    // setup_talk();
    setup_interface();

    char* note = strdup("EHdjfisjidfj\nuijhasuhdfuhuhuhua\n\njuhauhsufdhuahdfiajfi\n");
    editor_set_note(note);

    while(true) {
        draw_interface();
        usleep(500);
    }
}
