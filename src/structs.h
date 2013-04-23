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
