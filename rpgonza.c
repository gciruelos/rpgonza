/*
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>

#ifdef _WIN32
	#define SLEEP Sleep(1500)
#elif __linux__
	#define SLEEP sleep(1.5)
#endif

#define MAX_INPUT 100
#define MAX_DUNGEON 15

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

char map[50][50];

struct point {
	int x;
	int y;
};

struct stats {
	int health;
	int level;
	int experience;
	//int attack;
	//int defense;
};

struct mon {
	int x;
	int y;
	int health;
	int attack;
	int defense;
	int alive;
};


struct point myspot;
struct stats mystats = {100, 2, 0};
int inventory[6] = {0,0,0,0,0};
/*money, sword, shield, rocks, potatoes*/

struct mon monster1, monster2;

void printexperiencebar();
char tile_generation(int h, int l);
void dungeon_generation();
void dungeon_cleaning();
void assignspot();
void assignmonster();
int printmap ();
int analyze (char c);
void changemap(int x, int y, char c);
void experience_gain(int exp);
void action_walk(char c);
void action_open_chest();
void action_press_button();
void walk_monster(struct mon *monster);
void show_inventory();
int is_battle();
void battle_mode(struct mon *monster);
int battle_input(char n, struct mon *monster);
void monster_turn(struct mon *monster);
int getch (void);
int rand_lim(int limit);


int main (){
	char input;
	
	srand(time(0));
	
	while (1) {
		dungeon_cleaning();
		dungeon_generation();
		assignspot();
		assignmonster();
		while (!analyze(input)){
			is_battle();
			system("clear");
			printf("\n");
			printmap();
			input = getch();
			walk_monster(&monster1);
			walk_monster(&monster2);
		}   
	}
	
	return 0;
}


char tile_generation(int h, int l){
	int i, j;
	
	for (i = 1; i<h; i++){
		for (j = 1; j<l; j++){
			if (rand_lim(20)==1){
				map[i][j] = '.';
			}
			else if (rand_lim(30)==1){
				map[i][j] = '?';
			}
			else if (rand_lim(500)==1){
				map[i][j] = 'S';
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


void assignmonster(){
	int a, b, i = 0, j = 0;
	
	a = maplh()/100;
	b = maplh()%100;
	
	//printf("%i\t%i\n", b, a);
	
	if (a*b>25){
		while (1){
			if (i<=1 || i>=b-1 || j<=1 || j>=a-1){
				i = rand_lim(30);
				j = rand_lim(30);
			}
			else if (j == myspot.y && i == myspot.x){
				i = rand_lim(30);
				j = rand_lim(30);
			}
			else{
				break;
			}
		}

		monster1.x = i;
		monster1.y = j;
		monster1.health = rand_lim(30)+80;
		monster1.alive = 1;
	}
	
	if ((a*b)>=100){
		i = j = 0;
		while (1){
			if (i<=1 || i>=b-1 || j<=1 || j>=a-1){
				i = rand_lim(30);
				j = rand_lim(30);
			}
			else if (j == myspot.y && i == myspot.x){
				i = rand_lim(30);
				j = rand_lim(30);
			}
			else{
				break;
			}
		}

		monster2.x = i;
		monster2.y = j;
		monster2.health = rand_lim(30)+80;
		monster2.alive = 1;
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
	int a = 0, b;
	//printf("DEBUG    LE YO: x=%i y=%i\n\n", myspot.x, myspot.y);
	//printf("DEBUG MONSTER1: x=%i y=%i\n\n", monster1.x, monster1.y);
	//printf("DEBUG MONSTER2: x=%i y=%i\n\n", monster2.x, monster2.y);
	printf(" ");
	for (a = 0;;a++){
		for (b = 0;; b++){
			if (map[a][b] == 'E'){
				printf("\n Health: %i\tLevel: %i\n Experience: ", mystats.health, mystats.level);
				printexperiencebar();
				return 0;
			}
			else if (a == myspot.x && b == myspot.y){
				printf("\033[22;34m%c \x1b[0m", '&');
			}
			else if (a == monster1.x && b == monster1.y && monster1.alive == 1){
				printf("%c ", '@');
			}
			else if (a == monster2.x && b == monster2.y && monster2.alive == 1){
				printf("%c ", '@');
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
		else if (map[myspot.x][myspot.y] == 'S'){
			printf("Shop isn\'t available yet.");
			SLEEP;
		}
		else if (map[myspot.x][myspot.y] == '>'){
			return 1;
		}
	}
	return 0;
}


void show_inventory(){
	printf("You have %i$, a level %i sword, a level %i shield, %i rocks and %i potatoes...",\
			inventory[0],inventory[1],inventory[2],inventory[3], inventory[4]);
	SLEEP;
}


void action_walk(char c){
	if (c == 'w'){
		myspot.x-=1;
		if (map[myspot.x][myspot.y] == '#'){
			myspot.x+=1;
		}
	}
	else if (c == 's'){
		myspot.x+=1;
		if (map[myspot.x][myspot.y] == '#'){
			myspot.x-=1;
		}
	}
	else if (c == 'a'){
		myspot.y-=1;
		if (map[myspot.x][myspot.y] == '#'){
			myspot.y+=1;
		}
	}
	else if (c == 'd'){
		myspot.y+=1;
		if (map[myspot.x][myspot.y] == '#'){
			myspot.y-=1;
		}
	}
}


void action_open_chest(){
	int r = rand_lim(15), n = rand_lim(5), q;
	if (n == 0){
		printf("Sorry, there was nothing in the chest.\n");
	}
	else if (n == 1 || n == 2){
		printf("You found experienge!\nYour experience has been increased by %i.\n", r);
		experience_gain(r);
	}
	else if (n == 3){
		printf("You found bag of coins!\nIt cointains %i coins.\n", r);
		inventory[0]+=r;
	}
	else if (n == 4){
		q = r/4;
		printf("You found bag of rocks!\nIt cointains %i rocks.\n", q);
		inventory[4]+=q;
	}
	else if (n == 5){
		q = r/3;
		printf("You found %i potatoes!\n", q);
		inventory[5]+=q;
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


void walk_monster(struct mon *monster){
	if (monster->alive){
		int c = rand_lim(3);
		if (!c){
			monster->x-=1;
			if (map[monster->x][monster->y] == '#'){
				monster->x+=1;
			}
		}
		else if (c == 1){
			monster->x+=1;
			if (map[monster->x][monster->y] == '#'){
				monster->x-=1;
			}
		}
		else if (c == 2){
			monster->y-=1;
			if (map[monster->x][monster->y] == '#'){
				monster->y+=1;
			}
		}
		else if (c == 3){
			monster->y+=1;
			if (map[monster->x][monster->y] == '#'){
				monster->y-=1;
			}
		}
		else{
			printf("_DEBUG: RANDOM GENERATOR IN WALL_MONSTER");
		}
	}
}


int is_battle(){
	int a = maplh()/100, b = maplh()%100;
	
	if (a*b<=25){
		return 0;
	}
	
	else if (a*b>25){
		if ((myspot.x+1==monster1.x || myspot.x-1==monster1.x) && (myspot.y+1==monster1.y || myspot.y-1==monster1.y)){
			battle_mode(&monster1);
			return 1;
		}
		else if (myspot.x==monster1.x && myspot.y==monster1.y){
			battle_mode(&monster1);
			return 1;
		}
		else return 0;
	}
	
	else if (a*b>=100){
		if ((myspot.x+1==monster1.x || myspot.x-1==monster1.x) && (myspot.y+1==monster1.y || myspot.y-1==monster1.y)){
			battle_mode(&monster1);
			return 1;
		}
		else if (myspot.x==monster1.x && myspot.y==monster1.y){
			battle_mode(&monster1);
			return 1;
		}
		else if ((myspot.x+1==monster2.x || myspot.x-1==monster2.x) && (myspot.y+1==monster2.y || myspot.y-1==monster2.y)){
			battle_mode(&monster2);
			return 1;
		}
		else if (myspot.x==monster2.x && myspot.y==monster2.y){
			battle_mode(&monster1);
			return 1;
		}
		else return 0;
	}
	else return 0;
}


void battle_mode(struct mon *monster){
	char input = 0;
	int i = 0;
	
	int l = mystats.level + rand_lim(6) - 3;
	
	monster->attack = l*4;
	monster->defense = l*4;
	
	while (mystats.health>0 && (monster->health)>0) {
		system("clear");
		printf("\n");
		printf("BATTLE MODE\n");
		printf("What do you want to do?\n");
		printf("1. Attack\n2. Flee\n3. Throw rock\n\n");
		printf("Your health is %i/100\n", mystats.health);
		printf("DEBUG: MONSTER LIFE %i\n", monster->health);
		while(!i){		
			input = getchar();
			i = battle_input(input, monster);
		}
		if (i==2){
			break;
		}
		i = 0;
		
		monster_turn(monster);
	}
	if (mystats.health<=0){
		printf("\n\nYOU LOST THE GAME.\n\n");
		exit(0);
	}
	else if ((monster->health)<=0){
		printf("\n\nYOU DEFEATED THE MONSTER.\n\n");
		monster->x = '\0';
		monster->y = '\0';
	}
	else printf("You fleed successfully...");
	
	SLEEP;
	
}


int battle_input(char n, struct mon *monster){
	int attack = mystats.level*5 + inventory[1]*3;
	int att = 0;
	
	if (n == '1'){
		att = attack - monster->defense + rand_lim(25);
		if (att>0){
			monster->health -= att;
		}
		return 1;
	}
	else if (n == '2'){
		if (!rand_lim(3)){
			return 2;}
		else return 1;
	}
	else if (n == '3'){
		if (!inventory[4]){
			printf("You don\'t have no rocks.\n");
			return 0;
		}
		else{
			att = rand_lim(40) - monster->defense + rand_lim(25);
			if (att>0){
				monster->health -= att;
			}
			return 1;
		}
	}
	else return 0;
}

		
void monster_turn(struct mon *monster){
	int defense = mystats.level*5 + inventory[2]*3;
	int monster_attack = (monster->attack - defense + rand_lim(15));
	
	if (monster_attack>=0){
		mystats.health -= monster_attack;
	}
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

