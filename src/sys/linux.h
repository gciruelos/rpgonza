#include <unistd.h>
#include <termios.h>

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


void clear(){
	system("clear");
}
