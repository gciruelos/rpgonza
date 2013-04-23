/*
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "structs.h"
#include "math.h"

#ifdef _WIN32
	#define SLEEP Sleep(1500)
	#define CLEAR system("cls")
#elif __linux__
	#include "sys/linux.h"
	#define SLEEP sleep(1.5)
	#define CLEAR system("clear")
	
#endif

#define MAX_DUNGEON 20
#define MONSTERS_PER_DUNGEON 0.015

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
int getch (void);
int rand_lim(int limit);


int main (){
	char input = 0;
	
	srand(time(0));
	
	while (1) {
		dungeon_no++;
		dungeon_cleaning();
		dungeon_generation();
		assignspot();
		cleanmonster();
		assignmonster();
		while (!analyze(input)){
			CLEAR;
			printf("\n");
			(void) printmap();
			walk_monster();
			input = getch();
		}   
	}
	return 0;
}


void tile_generation(int h, int l){
	int i, j;
	
	float difficulty = dungeon_no/50.0;
	
	for (i = 1; i<h; i++){
		for (j = 1; j<l; j++){
			if (rand_lim(30/difficulty)==1){
				map[i][j] = '.';
			}
			else if (rand_lim(30/difficulty)==1){
				map[i][j] = '?';
			}
			else if (rand_lim(1000/difficulty)==1){
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
	int a, b, i = 0, j = 0, h, lvl = mystats.level;
	
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
		monsters[h].x = i;
		monsters[h].y = j;
		monsters[h].health = rand_lim(30)+80;
		monsters[h].attack = 2*lvl - rand_lim(lvl/2) + lvl/4;
		monsters[h].defense = 2*lvl - rand_lim(lvl/2) + lvl/4;
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
	/*printf("DEBUG    LE YO: x=%i\ty=%i\n", myspot.x, myspot.y);
	int h;
	for (h = 0; monsters[h].x!='\0'; h++){
		printf("DEBUG MONSTER%i: x=%i\ty=%i\t%i\t%i\t%i\t%i\t%c\n", h, monsters[h].x, monsters[h].y, monsters[h].health, monsters[h].attack, monsters[h].defense, monsters[h].alive, monsters[h].type);
	}
	printf("\n");*/
	
	int a = 0, b, c;
		
	printf(" ");
	for (a = 0;;a++){
		for (b = 0;; b++){
			if (map[a][b] == 'E'){
				printf("\n Health: %i\tLevel: %i\n Experience: ", mystats.health, mystats.level);
				printexperiencebar();
				printf("Dungeon number: %i\n\n", dungeon_no);
				return 0;
			}
			else if (istheremonster(a, b)+1){
				c = istheremonster(a, b);
				if (monsters[c].health>=50){
					printf("\033[1;31m%c \x1b[0m", monsters[c].type);
				}
				else if (monsters[c].health<50 && monsters[c].health>=20){
					printf("\033[0;31m%c \x1b[0m", monsters[c].type);
				}
				else if (monsters[c].health<20){
					printf("\033[2;31m%c \x1b[0m", monsters[c].type);
				}
			}
			else if (a == myspot.x && b == myspot.y){
				printf("\033[52;34m%c \x1b[0m", '&');
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
	
	do{
		if (mystats.experience+exp>=max_experience){
			mystats.level++;
			exp-=(max_experience-mystats.experience);
		}
		else{
			mystats.experience+=exp;
			break;
		}
	} while (exp>0);
	
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
	
	CLEAR;
	
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
	
	changemap(myspot.x, myspot.y, ' ');
	
	SLEEP;
}


void action_press_button(){
	int i, j;
	char guess[20];
	
	i = rand_lim(rand_lim(50));
	
	j = rand_lim(i);
	
	CLEAR;
	
	
		printf("\n\nGuess a number between 0 and %i.\n", i);
		scanf("%s", &guess);
	
	if (atoi(guess)!=j){
		printf("Nope, the number was %i.\n", j);
	}
	else{
		printf("Congrats.\n");
		if (j%2){
			printf("You earned %i points of experience.\n", i*mystats.level+1);
			experience_gain(i*mystats.level);
		}
		else{
			if (i%2){
				printf("Your sword has been improved.\n");
				inventory[1]++;
			}
			else{
				printf("Your shield has been improved.\n");
				inventory[2]++;
			}
		}
	}
	
	changemap(myspot.x, myspot.y, ' ');
	
	SLEEP;
	
}


void action_attack_sword(){
	int x = myspot.x, y = myspot.y, a;
	int attack = 2*mystats.level;
	
	for (a = 0; monsters[a].x!='\0'; a++){
		if (abs(monsters[a].x-x)<=1 && abs(monsters[a].y-y)<=1){
			break;
		}
	}
	
	int damage = attack + -monsters[a].defense + rand_lim(30) - 20 + inventory[1];
	
	if (damage>=0){
		monsters[a].health -= damage;
	}
	
	
	if (monsters[a].health<=0){	
		printf("You have killed a monster! You have earned some experience.\n");
		experience_gain((monsters[a].attack+1)*(monsters[a].defense+1));
			
		/*kill monster*/
		monsters[a].x = 90; /*To avoid bugs*/
		monsters[a].y = '\0';
		monsters[a].health = '\0';
		monsters[a].attack = '\0';
		monsters[a].defense = '\0';
		monsters[a].alive = '\0';
		monsters[a].type = '\0';
	}
}


void walk_monster(){
	int a;
	struct mon *monster;
	
	for (a = 0; monsters[a].x; a++){
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
	}

	for (a = 0; monsters[a].x!='\0'; a++){
		if (abs(monsters[a].x-myspot.x)<=1 && abs(monsters[a].y-myspot.y)<=1){
			monster_turn(&monsters[a]);
		}
	}
}

		
void monster_turn(struct mon *monster){
	int defense = 2*mystats.level;
	int damage = monster->attack - defense + rand_lim(30) - 15 - inventory[2];
	
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


int istheremonster(int x, int y){
	int a;
	for (a = 0; monsters[a].x!='\0'; a++){
		if (monsters[a].x == x && monsters[a].y == y && monsters[a].alive){
			return a;
		}
	}
	return -1;
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
