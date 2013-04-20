/*
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include "math.h"

#ifdef _WIN32
	#define SLEEP Sleep(1500)
#elif __linux__
	#define SLEEP sleep(1.5)
#endif

#define MAX_DUNGEON 20
#define MONSTERS_PER_DUNGEON 0.015

/* MAP EXAMPLE
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
							{'#','#','#','#','#','#','#','E'}};*/

char map[30][30];

struct point {
	int x;
	int y;
};

struct stats {
	int health;
	int level;
	int experience;
};

struct mon {
	int x;
	int y;
	char type;
	/* O Orc, D Dragon, G Goblin, W Witch, B Beast, M Minotaur*/
	int health;
	int attack;
	int defense;
	int alive;
};

struct mon monsters[6];


struct point myspot;
struct stats mystats = {100, 2, 0};
int inventory[5] = {0, 0, 0, 0, 0};
/*money, sword, shield, rocks, potatoes*/


char tile_generation(int h, int l);
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
char istheremonster(int x, int y);
int getch (void);
int rand_lim(int limit);


int main (){
	char input;
	
	srand(time(0));
	
	while (1) {
		dungeon_cleaning();
		dungeon_generation();
		assignspot();
		cleanmonster();
		assignmonster();
		while (!analyze(input)){
			system("clear");
			printf("\n");
			printmap();
			walk_monster();
			input = getch();
		}   
	}
	
	return 0;
}


char tile_generation(int h, int l){
	int i, j;
	
	for (i = 1; i<h; i++){
		for (j = 1; j<l; j++){
			if (rand_lim(30)==1){
				map[i][j] = '.';
			}
			else if (rand_lim(40)==1){
				map[i][j] = '?';
			}
			else if (rand_lim(1000)==1){
				map[i][j] = '%';
			}
		}
	}
	i = j = 0;
	while (i==0 || i>(h-2) || j==0 || j>(l-2)){
		i = rand_lim(h-1);
		j = rand_lim(l-1);
	}
	map[i][j] = '>';
}


void dungeon_generation(){
	int dungeonl = 0, dungeonh = 0;
	int h, l;
	
	while (dungeonl<6 || dungeonh<5){
		dungeonl = rand_lim(MAX_DUNGEON);
		dungeonh = rand_lim(MAX_DUNGEON);
	}
	
	for (h = 0; h<=dungeonh; h++){
		l = 0;
		map[h][l] = '#';
		l = dungeonl;
		map[h][l] = '#';
		l = dungeonl+1;
		map[h][l] = '\n';
	}
	for (l = 0; l<=dungeonl; l++){
		h = 0;
		map[h][l] = '#';
		h = dungeonh;
		map[h][l] = '#';
	}
	map[dungeonh][dungeonl+1] = 'E';
	for (h = 0; h<=dungeonh; h++){
		for (l = 0; l<=dungeonl; l++){
			if (map[h][l] != '#' && map[h][l] != '\n' && map[h][l] != 'E'){
				map[h][l] = ' ';
			}
		}
	}
	
	tile_generation(dungeonh, dungeonl);
	
}


void dungeon_cleaning(){
	int i, j;
	
	for (i = 0; i<=50; i++){
		for (j = 0; j<=50; j++){
			map[i][j] = '\0';
		}
	}
}


void assignspot(){
	int a, b;
	
	a = maplh()/100;
	b = maplh()%100;
	
	myspot.x = b/2;
	myspot.y = a/2;
}


void cleanmonster(){
	int h;
	
	for (h = 0; h<=6; h++){
		monsters[h].x = '\0';
		monsters[h].y = '\0';
		monsters[h].health = '\0';
		monsters[h].attack = '\0';
		monsters[h].defense = '\0';
		monsters[h].alive = '\0';
		monsters[h].type = '\0';
	}
}


void assignmonster(){
	int a, b, i = 0, j = 0, h;
	
	a = maplh()/100;
	b = maplh()%100;
	
	int quantity = MONSTERS_PER_DUNGEON*a*b;
	
	printf("%i\t%i\n\n", a, b);
	
	for (h = 0; h<quantity; h++){
		while (1){
			if (i<1 || i>=b-1 || j<1 || j>=a-1 || (j == myspot.y && i == myspot.x)){
				i = rand_lim(30);
				j = rand_lim(30);
			}
			else break;
		}
		int att = mystats.level*5 - rand_lim(30), def = mystats.level*5 - rand_lim(30);
		monsters[h].x = i;
		monsters[h].y = j;
		monsters[h].health = rand_lim(30)+80;
		(att>0)? (monsters[h].attack = att) : (monsters[h].attack = 1);
		(def>0)? (monsters[h].defense = def) : (monsters[h].defense = 1);
		monsters[h].alive = 1;
		monsters[h].type = randommonster();
		
		//printf("%i\t%i\t%i\t%i\t%c\n", monsters[h].x, monsters[h].y, monsters[h].health, monsters[h].alive, monsters[h].type);
		i = j = 0;
	}
}


void printexperiencebar(){
	char experiencebar[19] = {'[',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',']'};
	int i, lvl = mystats.level, percentage = (mystats.experience*12)/(lvl*lvl*lvl);
	for (i = 1; i< percentage; i++){
		experiencebar[i] = '-';
	}
	printf("%s\n\n", experiencebar);
}


int printmap(){
	printf("DEBUG    LE YO: x=%i\ty=%i\n", myspot.x, myspot.y);
	int h;
	for (h = 0; monsters[h].x!='\0'; h++){
		printf("DEBUG MONSTER%i: x=%i\ty=%i\t%i\t%i\t%i\t%i\t%c\n", h, monsters[h].x, monsters[h].y, monsters[h].health, monsters[h].attack, monsters[h].defense, monsters[h].alive, monsters[h].type);
	}
	printf("\n");
	
	int a = 0, b;
		
	printf(" ");
	for (a = 0;;a++){
		for (b = 0;; b++){
			if (map[a][b] == 'E'){
				printf("\n Health: %i\tLevel: %i\n Experience: ", mystats.health, mystats.level);
				printexperiencebar();
				return 0;
			}
			else if (istheremonster(a, b)){
				printf("%c ", istheremonster(a, b));
			}
			else if (a == myspot.x && b == myspot.y){
				printf("\033[22;34m%c \x1b[0m", '&');
			}
			else{
				printf("%c ", map[a][b]);
				if (map[a][b] == '\n'){
					
					break;
				}
			}
		}
	}
}

		
void changemap(int x, int y, char c){
	map[x][y] = c;
}


void experience_gain(int exp){
	int lvl = mystats.level;
	int max_experience = (lvl*lvl*lvl); /*Stolen from Pokemon. Fast*/
	
	if (mystats.experience+exp>=max_experience){
		mystats.level++;
		exp-=(max_experience-mystats.experience);
		if (mystats.experience+exp>=max_experience){
			mystats.level++;
			exp-=(max_experience-mystats.experience);
			if (mystats.experience+exp>=max_experience){
				mystats.level++;
				exp-=(max_experience-mystats.experience);
				if (mystats.experience+exp>=max_experience){
					mystats.level++;
					exp-=(max_experience-mystats.experience);
					mystats.experience = exp;
				}
			}
		}
	}
	else{
		mystats.experience+=exp;
	}
}


int analyze (char c){
	if (c == 'w' || c == 'a' || c == 's' || c == 'd'){
		action_walk (c);
	}
	else if (c == 'i'){
		show_inventory ();
	}
	else if (c == 'e'){
		if(map[myspot.x][myspot.y] == '?'){
			action_open_chest();
		}
		else if (map[myspot.x][myspot.y] == '.'){
			action_press_button();
		}
		else if (map[myspot.x][myspot.y] == '%'){
			printf("Shop isn\'t available yet.");
			SLEEP;
		}
		else if (map[myspot.x][myspot.y] == '>'){
			return 1;
		}
	}
	/*space is sword, x rock*/
	else if (c == ' ' && nearbymonster()){
		action_attack_sword();
	}
	return 0;
	
}


void end (char type){
	/* O Orc, D Dragon, G Goblin, W Witch, B Beast, M Minotaur*/
	char monster[30];
	switch (type){
		case 'O':
			strcpy(monster, " |         ORC         |  \n");
			break;
		case 'D':
			strcpy(monster, " |       DRAGON        |  \n");
			break;
		case 'G':
			strcpy(monster, " |       GOBLIN        |  \n");
			break;
		case 'W':
			strcpy(monster, " |        WITCH        |  \n");
			break;
		case 'B':
			strcpy(monster, " |        BEAST        |  \n");
			break;
		case 'M':
			strcpy(monster, " |      MINOTAUR       |  \n");
			break;
	}
	
	system("clear");
	
	printf("     _______________      \n");
	printf("    /               \\    \n");
	printf("   /                 \\   \n");
	printf("  /                   \\  \n");
	printf(" |      HERE LIES      |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |     KILLED BY A     |  \n");
	printf("%s", monster);
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" |                     |  \n");
	printf(" -----------------------  \n");
	
	exit(0);
}


void show_inventory(){
	printf("You have %i$, a level %i sword, a level %i shield, %i rocks and %i potatoes...",\
			inventory[0],inventory[1],inventory[2],inventory[3], inventory[4]);
			/*money, sword, shield, rocks, potatoes*/
	SLEEP;
}


void action_walk(char c){
	int i;
	
	if (c == 'w'){
		myspot.x-=1;
	}
	else if (c == 's'){
		myspot.x+=1;
	}
	else if (c == 'a'){
		myspot.y-=1;
	}
	else if (c == 'd'){
		myspot.y+=1;
	}
	
	for (i = 0; monsters[i].x!='\0'; i++){
		if ((myspot.x == monsters[i].x && myspot.y == monsters[i].y) || (map[myspot.x][myspot.y] == '#')){
			if (c == 'w'){
				myspot.x+=1;
				break;
					}
			else if (c == 's'){
				myspot.x-=1;
				break;
			}
			else if (c == 'a'){
				myspot.y+=1;
				break;
			}
			else if (c == 'd'){
				myspot.y-=1;
				break;
			}
		}
	}			
}


void action_open_chest(){
	int r = rand_lim(4), n = rand_lim(7);
	if (n == 0){
		printf("Sorry, there was nothing in the chest.\n");
	}
	else if (n == 1 || n == 2){
		printf("You found experienge!\nYour experience has been increased by %i.\n", r*mystats.level);
		experience_gain(r*mystats.level);
	}
	else if (n == 3 || n == 4){
		printf("You found some food! Your health has been restored.\n");
		mystats.health += (r+1)*10;
		if (mystats.health>100) mystats.health=100;
	}
	else if (n == 5){
		printf("You found bag of coins!\nIt cointains %i coins.\n", r);
		inventory[0]+=r;
	}
	else if (n == 6){
		printf("You found bag of rocks!\nIt cointains %i rocks.\n", r);
		inventory[3]+=r;
	}
	else if (n == 7){
		printf("You found %i potatoes!\n", r);
		inventory[4]+=r;
	}
	else{
		printf("_DEBUG: The random number genererated in the function action_open_chest is wrong: %i\n", n);
	}
	map[myspot.x][myspot.y] = ' ';
	SLEEP;
}


void action_press_button(){
	int i;
	for (i = 0; i<999; i++){
		printf("420 BLAZE IT FAGGOT\n");
	}
	SLEEP;
}


void action_attack_sword(){
	int x = myspot.x, y = myspot.y, a;
	int attack = mystats.level*3;
	
	for (a = 0; monsters[a].x!='\0'; a++){
		if (abs(monsters[a].x-x)<=1 && abs(monsters[a].y-y)<=1){
			break;
		}
	}
	
	int damage = attack + rand_lim(attack/6) - monsters[a].defense*2;
	
	if (damage>=0){
		monsters[a].health -= damage;
	}
	
	
	if (monsters[a].health<=0){
		monsters[a].alive = 0;
		printf("You have killed a monster! You have earned some experience.\n");
		experience_gain((monsters[a].attack+1)*(monsters[a].defense+1));
	}
	
}


void walk_monster(){
	int a;
	struct mon *monster;
	
	for (a = 0; monsters[a].x!='\0' && monsters[a].y!='\0'; a++){
		monster = &(monsters[a]);
		if (monster->alive){
			int c = rand_lim(3);
			if (!c){
				monster->x-=1;
				if (map[monster->x][monster->y] == '#' || (monster->x==myspot.x && monster->y==myspot.y)){
					monster->x+=1;
				}
			}
			else if (c == 1){
				monster->x+=1;
				if (map[monster->x][monster->y] == '#' || (monster->x==myspot.x && monster->y==myspot.y)){
					monster->x-=1;
				}
			}
			else if (c == 2){
				monster->y-=1;
				if (map[monster->x][monster->y] == '#' || (monster->x==myspot.x && monster->y==myspot.y)){
					monster->y+=1;
				}
			}
			else if (c == 3){
				monster->y+=1;
				if (map[monster->x][monster->y] == '#' || (monster->x==myspot.x && monster->y==myspot.y)){
					monster->y-=1;
				}
			}
			else{
				printf("_DEBUG: RANDOM GENERATOR IN WALK_MONSTER");
			}
		}
	
		else{
			monsters->x = 90; /*To avoid bugs*/
			monsters->y = 90;
			monsters->health = '\0';
			monsters->attack = '\0';
			monsters->defense = '\0';
			monsters->alive = '\0';
			monsters->type = '\0';
		}
	}

	for (a = 0; monsters[a].x!='\0'; a++){
		if (abs(monsters[a].x-myspot.x)<=1 && abs(monsters[a].y-myspot.y)<=1){
			monster_turn(&monsters[a]);
		}
	}
}

		
void monster_turn(struct mon *monster){
	int defense = mystats.level*2;
	int damage = monster->attack*2 + rand_lim(monster->attack/6) - defense;
	
	if (damage>=0){
		mystats.health -= damage;
	}
	
	if (mystats.health<=0) end(monsters->type);
}
	

int maplh(){
	/*Returns the height and the length of the map*/
	int a, b;
	for (a = 1; a<20; a++){
		if (map[0][a]=='\n') break;
	}
	
	for (b = 1; b<20; b++){
		if (map[b][0]=='\0') break;
	}
	
	return a*100+b;
}


int nearbymonster(){
	int x = myspot.x, y = myspot.y, a;
	
	for (a = 0; monsters[a].x!='\0'; a++){
		if (abs(monsters[a].x-x)<=1 && abs(monsters[a].y-y)<=1){
			if (monsters[a].alive) return 1;
		}
	}
	return 0;
}


char randommonster(){
	int a = rand_lim(5);
	
	/* O Orc, D Dragon, G Goblin, W Witch, B Beast, M Minotaur*/
	
	if (a == 0) return 'O';
	else if (a == 1) return 'D';
	else if (a == 2) return 'G';
	else if (a == 3) return 'W';
	else if (a == 4) return 'B';
	else if (a == 5) return 'M';
}


char istheremonster(int x, int y){
	int a;
	for (a = 0; monsters[a].x!='\0'; a++){
		if (monsters[a].x == x && monsters[a].y == y && monsters[a].alive){
			return monsters[a].type;
		}
	}
	return '\0';
}
	

int getch(){
	/*http://forums.debian.net/viewtopic.php?f=8&t=59524*/
	int ch;
	struct termios origterm, tmpterm;

	tcgetattr(STDIN_FILENO, &origterm);
	tmpterm = origterm;
	tmpterm.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &tmpterm);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &origterm);

	return ch;
}


int rand_lim(int limit) {
	/* http://stackoverflow.com/questions/2999075/generate-a-random-number-within-range/2999130#2999130
	 * Somewhat modified.*/
	int r = rand();
	int divisor = RAND_MAX/(limit+1), retval;

	do{
		retval = r / divisor;
	} while (retval > limit);

	return retval;
}

