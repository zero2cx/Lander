#include "headers/getch.h"
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
	int startPoint = 15;
	int rockstart = 0;
	int loops = 0;
	int consoleGraph = 1;
	int chKBHIT;
	srand(time(0));
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE
	clear();
	int randomXn = rand()%(w.ws_col - 6)+3;
	int newRock = 0;
	int wtf = 0;


	nodelay(stdscr, TRUE);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);


	while(true) {
		char key;
		struct winsize w;

		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE
		clear();


		while( true ){

			clear();

			if ( rockstart > w.ws_row ){
				++wtf;
				rockstart = 0;
				newRock = 1;
			}

			if (kbhit()){
				key = getch();
				break;
			}

			if ( newRock == 1 ) {
				srand(time(0));
				randomXn = rand()%(w.ws_col - 6)+3;
				newRock = 0;
			}
			for (int i = 0; i < w.ws_row; ++i){     // BORDERS
				mvprintw(i,3,"|");
				mvprintw(i,w.ws_col - 3,"|");
			}
			if ( consoleGraph == 1 ){
				mvprintw(0,4,"lines %d\n", w.ws_row);
				mvprintw(1,4,"columns %d\n", w.ws_col);
				mvprintw(2,4,"Cursor at x:%i", startPoint);
				mvprintw(3,4,"Rock y:%i x:%i", rockstart, randomXn);
				mvprintw(4,4,"Loops %i", loops);
				if (kbhit()){
					mvprintw(5,4,"kbhit is at 1");
				} else{
					mvprintw(5,4,"kbhit is at 0");
				}
				mvprintw(6,4,"newRock val:%i",newRock);
				mvprintw(7,4,"wtf:%i", wtf);
				mvprintw(8,4,"kbhit ch:%i", chKBHIT);
			}
			if ( (startPoint == randomXn || startPoint + 1 == randomXn || startPoint + 2 == randomXn) && (rockstart == w.ws_row - 3) ){
				goto GOVER;
			}
			refresh();
			mvprintw(w.ws_row - 3,startPoint,"/A\\");
			mvprintw(rockstart,randomXn,"X");
			++rockstart;                // END ROCK

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

			mvprintw(w.ws_row - 3,startPoint,"/A\\");

			if ( (startPoint == randomXn || startPoint + 1 == randomXn || startPoint + 2 == randomXn) && (rockstart == w.ws_row - 3) ){
				goto GOVER;
			}
			refresh();
		}

		clear();
		mvprintw(rockstart,randomXn,"X");

		for (int i = 0; i < w.ws_row; ++i){     // BORDERS
			mvprintw(i,3,"|");
			mvprintw(i,w.ws_col - 3,"|");
		}


		if ( key == 'z' ){
			if ( startPoint == 4 ){
				continue;
			} else{
				startPoint = startPoint - 1;
				mvprintw(w.ws_row - 3,startPoint,"/A\\");
			}
		} else if ( key == 'x' ){
			if ( startPoint == w.ws_col - 6 ){
				continue;
			} else{
				startPoint = startPoint + 1;
				mvprintw(w.ws_row - 3,startPoint,"/A\\");
			}
		} else {
			continue;
		}

		if ( consoleGraph == 1 ){
			mvprintw(0,4,"lines %d\n", w.ws_row);
			mvprintw(1,4,"columns %d\n", w.ws_col);
			mvprintw(2,4,"Cursor at x:%i", startPoint);
			mvprintw(3,4,"Rock y:%i x:%i", rockstart, randomXn);
			mvprintw(4,4,"Loops %i", loops);
			if (kbhit()){
				mvprintw(5,4,"kbhit is at 1");
			} else{
				mvprintw(5,4,"kbhit is at 0");
			}
			mvprintw(6,4,"newRock val:%i",newRock);
			mvprintw(7,4,"wtf:%i", wtf);
			mvprintw(8,4,"kbhit ch:%i", chKBHIT);
		}

		if ( (startPoint == randomXn || startPoint + 1 == randomXn || startPoint + 2 == randomXn) && (rockstart == w.ws_row - 3) ){
			goto GOVER;
		}

		refresh();
	}
	GOVER:refresh();
	clear();
	mvprintw(10,10,"GAME OVER");
	refresh();
	localGetch();
	endwin();
	return 0;
}
