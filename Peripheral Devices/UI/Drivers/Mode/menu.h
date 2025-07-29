#ifndef MENU_H
#define MENU_H

#include "mode_tree.h"
#include "oled.h"

#define END_X 128 // End X coordinate for the OLED display

// Function prototypes for menu operations
void menu_init(void);
void menu_display(ModeTree *modeTree);
void menu_function(void);
void menu_select(ModeTree *modeTree);

bool is_menu_node(ModeTree *node); // Check if the node is a menu node
#endif // MENU_H