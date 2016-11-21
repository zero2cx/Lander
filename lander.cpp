#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>


int kbhit(void){
	int ch = getch();

	if (ch != ERR) {
		ungetch(ch);
		return 1;
	} else {
		return 0;
	}
}


void sleep_ms(int milliseconds){
#ifdef WIN32
	Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
#else
	usleep(milliseconds * 1000);
#endif
}


int main(){


	initscr();
	curs_set(0);
	int ship_X = 15;
	int rock_Y = 0;
	int loops = 0;
	int consoleGraph = 1;
	int chKBHIT;
	srand(time(0));
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE
	clear();
	int rock_X = rand()%(w.ws_col - 7)+4;
	int needsRock = 0;
	int wtf = 0;
	bool shoot = 0;
	int shoot_X = -1;
	int shoot_Y = -1;

	nodelay(stdscr, TRUE);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	while(true) {
		char key;
		struct winsize w;

		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE
		clear();

		while( true ){
			clear();
			if ( rock_Y > w.ws_row ){
				++wtf;
				rock_Y = 0;
				needsRock = 1;
			}
			if ( rock_Y == shoot_Y && rock_X == shoot_X ){
				++wtf;
				rock_Y = 0;
				needsRock = 1;
				shoot = false;
				shoot_X = -1;
				shoot_Y - 1;
			}

			if (kbhit()){
				key = getch();
				break;
			}

			if ( needsRock == 1 ) {
				srand(time(0));
				rock_X = rand()%(w.ws_col - 7)+4;
				needsRock = 0;
			}
			for (int i = 0; i < w.ws_row; ++i){     // BORDERS
				mvprintw(i,3,"|");
				mvprintw(i,w.ws_col - 3,"|");
			}
			if ( consoleGraph == 1 ){
				mvprintw(0,4,"lines %d\n", w.ws_row);
				mvprintw(1,4,"columns %d\n", w.ws_col);
				mvprintw(2,4,"Cursor at x:%i", ship_X);
				mvprintw(3,4,"Rock y:%i x:%i", rock_Y, rock_X);
				mvprintw(4,4,"Loops %i", loops);
				if (kbhit()){
					mvprintw(5,4,"kbhit is at 1");
				} else{
					mvprintw(5,4,"kbhit is at 0");
				}
				mvprintw(6,4,"needsRock val:%i",needsRock);
				mvprintw(7,4,"wtf:%i", wtf);
				mvprintw(8,4,"kbhit ch:%i", chKBHIT);
				mvprintw(9,4,"show at %i", shoot_Y);
			}
			if ( (ship_X == rock_X || ship_X + 1 == rock_X || ship_X + 2 == rock_X) && (rock_Y == w.ws_row - 3) ){
				goto GOVER;
			}
			if (shoot){
				mvprintw(shoot_Y,shoot_X+1,"*");
				--shoot_Y;
				if ( shoot_Y == 0 ){
					shoot = false;
					shoot_Y = w.ws_row-4;
				}
			}
			refresh();
			mvprintw(w.ws_row - 3,ship_X,"/A\\");
			mvprintw(rock_Y,rock_X,"X");
			++rock_Y;                // END ROCK

			if (kbhit()){
				key = getch();
				break;
			}

			sleep_ms(50);

			if (kbhit()){
				key = getch();
				break;
			}

			++loops;

			if (kbhit()){
				key = getch();
				break;
			}

			mvprintw(w.ws_row - 3,ship_X,"/A\\");

			if ( (ship_X == rock_X || ship_X + 1 == rock_X || ship_X + 2 == rock_X) && (rock_Y == w.ws_row - 3) ){
				goto GOVER;
			}
			refresh();
		}

		clear();
		mvprintw(rock_Y,rock_X,"X");

		for (int i = 0; i < w.ws_row; ++i){     // BORDERS
			mvprintw(i,3,"|");
			mvprintw(i,w.ws_col - 3,"|");
		}


		if ( key == 'z' ){
			if ( ship_X == 4 ){
				continue;
			} else{
				ship_X = ship_X - 1;
				mvprintw(w.ws_row - 3,ship_X,"/A\\");
			}
		} else if ( key == 'c' ){
			if ( ship_X == w.ws_col - 6 ){
				continue;
			} else{
				ship_X = ship_X + 1;
				mvprintw(w.ws_row - 3,ship_X,"/A\\");
			}
		} else if ( key == 'x'  && !shoot){
			shoot = true;
			shoot_X = ship_X;
			shoot_Y = w.ws_row-4;;
		} else {
			continue;
		}

		if ( consoleGraph == 1 ){
			mvprintw(0,4,"lines %d\n", w.ws_row);
			mvprintw(1,4,"columns %d\n", w.ws_col);
			mvprintw(2,4,"Cursor at x:%i", ship_X);
			mvprintw(3,4,"Rock y:%i x:%i", rock_Y, rock_X);
			mvprintw(4,4,"Loops %i", loops);
			if (kbhit()){
				mvprintw(5,4,"kbhit is at 1");
			} else{
				mvprintw(5,4,"kbhit is at 0");
			}
			mvprintw(6,4,"needsRock val:%i",needsRock);
			mvprintw(7,4,"wtf:%i", wtf);
			mvprintw(8,4,"kbhit ch:%i", chKBHIT);
			mvprintw(9,4,"show at %i", shoot_Y);
		}

		if ( (ship_X == rock_X || ship_X + 1 == rock_X || ship_X + 2 == rock_X) && (rock_Y == w.ws_row - 3) ){
			goto GOVER;
		}

		refresh();
	}
	GOVER:refresh();
	clear();
	mvprintw(w.ws_row/2,w.ws_col/2,"GAME OVER");
	refresh();
	sleep_ms(3000);
	endwin();
	return 0;
}
