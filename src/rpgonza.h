
/* MAP EXAMPLE;
 * char map[1000][1000] = {{'#','#','#','#','#','#','#','\n'},
							{'#',' ',' ',' ',' ',' ','#','\n'},
							{'#',' ','?',' ',' ','@','#','\n'},
							{'#',' ',' ',' ',' ',' ','#','\n'},
							{'#',' ',' ',' ',' ',' ','#','\n'},
							{'#',' ',' ',' ','.',' ','#','\n'},
							{'#','.',' ',' ',' ',' ','#','\n'},
							{'#',' ',' ','.',' ',' ','#','\n'},
							{'#',' ',' ',' ',' ',' ','#','\n'},
							{'#',' ',' ',' ','?',' ','#','\n'},
							{'#',' ',' ',' ',' ',' ','#','\n'},
							{'#','#','#','#','#','#','#','E'}};
 */
							
/* COMBAT MODE:
 * My attack   = 2.lvl
 * My defense  = 2.lvl
 * 
 * His attack  = 2.lvl - rand(lvl/2) + lvl/4
 * His defense = 2.lvl - rand(lvl/2) + lvl/4
 * 
 * Damage = attack -defense + rand(2) -1
 */

char map[30][30];

struct mon monsters[6];

int dungeon_no = 0;


struct point myspot;
struct stats mystats = {100, 2, 0};
int inventory[5] = {0, 0, 0, 0, 0};
/*money, sword, shield, rocks, potatoes*/


void messages(int i);
void tile_generation(int h, int l);
void dungeon_generation();
void dungeon_cleaning();
void assignspot();
void cleanmonster();
void assignmonster();
void printexperiencebar();
int printmap ();
void changemap(int x, int y, char c);
void experience_gain(int exp);
int analyze (char c);
void show_inventory();
void action_walk(char c);
void action_open_chest();
void action_press_button();
void action_attack_sword();
void walk_monster();
void monster_turn(struct mon *monster);
int maplh();
char randommonster();
int istheremonster(int x, int y);
int rand_lim(int limit);
