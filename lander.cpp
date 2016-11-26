#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <fstream>

struct rock_t {
	int velocity;
	int pos_X;
	int pos_Y;
	bool isActive;
	bool needsRock;
} rocks[0];

int nDigits(int x) {
	x = abs(x);
	return (x < 10 ? 1 : 
		(x < 100 ? 2 : 
		(x < 1000 ? 3 : 
		(x < 10000 ? 4 : 
		(x < 100000 ? 5 : 
		(x < 1000000 ? 6 : 
		(x < 10000000 ? 7 :
		(x < 100000000 ? 8 :
		(x < 1000000000 ? 9 :
		10)))))))));
}  

void destroyRock(int i) {
	rocks[i].pos_Y = 0;
	rocks[i].pos_X = -1;
	srand((time(0) * i) + time(0));
	int m_rand = rand()%10;
	//20% chance
	if(m_rand == 0 || m_rand == 1) {
		rocks[i].velocity = 2;
	}else{
		rocks[i].velocity = 1;
	}
}

void createRock(int i) {
	rock_t* r = &rocks[i];
	r->velocity = 1;
	r->pos_X = -1;
	r->pos_Y = 0;
	r->isActive = false;
	r->needsRock = false;
}

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


int main() {
	auto start_time = std::chrono::high_resolution_clock::now();
	initscr();
	curs_set(0);
	int ship_X = 15;
	int loops = 0;
	bool debugGraph = false;
	int chKBHIT;
	srand(time(0));
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE
	clear();
	int wtf = 0;
	int oldwtf = 0;
	bool shoot = 0;
	int shoot_X = -10;
	int shoot_Y = -10;
	int score = 0;
	int cooldownShot = 0;

	for(int i = 0; i < w.ws_col; i++) {
		createRock(i);
		srand((time(0) * i) + time(0));
		rocks[i].pos_X = rand()%(w.ws_col - 7)+4;
		rocks[i].isActive = false;
	}
	rocks[0].isActive = true;
	nodelay(stdscr, TRUE);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	start_color();
	while(true) {
		char key;
		struct winsize w;

		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE
		clear();

		while( true ){
			auto current_time = std::chrono::high_resolution_clock::now();
			auto second_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
			clear();
			for(int i = 0; i < w.ws_col; i++) {
				if (rocks[i].pos_Y > w.ws_row) {
					wtf += rocks[i].velocity;
					destroyRock(i);
					rocks[i].needsRock = 1;
				}
				if (rocks[i].isActive &&
					(rocks[i].pos_Y == shoot_Y || rocks[i].pos_Y == shoot_Y + 1 || rocks[i].pos_Y == shoot_Y - 1) &&
					(rocks[i].pos_X == shoot_X || rocks[i].pos_X == shoot_X + 1 || rocks[i].pos_X == shoot_X + 2)) {
					wtf += 2 * rocks[i].velocity;
					destroyRock(i);
					rocks[i].needsRock = 1;
					shoot = false;
					shoot_X = -10;
					shoot_Y = -10;
				}
				if (rocks[i].needsRock == 1) {
					rocks[i].pos_X = rand() % (w.ws_col - 7) + 4;
					srand((time(0) * i) + time(0));
					rocks[i].needsRock = 0;
				}
				if ((ship_X == rocks[i].pos_X || ship_X + 1 == rocks[i].pos_X || ship_X + 2 == rocks[i].pos_X)
					&& (rocks[i].pos_Y > (w.ws_row - 3))) {
					goto GOVER;
				}
			}

			if (kbhit()){
				key = getch();
				break;
			}

			if ( cooldownShot > 0 ) {
				--cooldownShot;
			}

			for (int i = 0; i < w.ws_row; ++i){     // BORDERS
				mvprintw(i,3,"|");
				mvprintw(i,w.ws_col - 3,"|");
			}
			if (debugGraph){
				mvprintw(2,4,"lines %d\n", w.ws_row);
				mvprintw(3,4,"columns %d\n", w.ws_col);
				mvprintw(4,4,"Cursor at x:%i", ship_X);
				mvprintw(6,4,"Loops %i", loops);
				if (kbhit()){
					mvprintw(7,4,"kbhit is at 1");
				} else{
					mvprintw(7,4,"kbhit is at 0");
				}
				mvprintw(9,4,"wtf:%i", wtf);
				mvprintw(10,4,"kbhit ch:%i", chKBHIT);
				mvprintw(11,4,"show at %i", shoot_Y);
				mvprintw(12,4,"oldwtf:%i", oldwtf);
				mvprintw(13, 4, "Colours:%i", has_colors());
				mvprintw(14,4,"cooldownShot: %i", cooldownShot);
			}
			mvprintw(0, 4, "Score:%i", score);
			mvprintw(1, 4, "Time:%i", second_time);
			mvprintw(0,w.ws_col - 3,"|");
			mvprintw(1,w.ws_col - 3,"|");
			mvprintw(2,w.ws_col - 3,"|");
			mvprintw(3,w.ws_col - 3,"|");
			if (shoot){
				mvprintw(shoot_Y,shoot_X+1,"*");
				--shoot_Y;
				if ( shoot_Y == 0 ){
					shoot = false;
					shoot_Y = -10;
				}
			}
			refresh();
			mvprintw(w.ws_row - 3,ship_X,"/A\\");
			for(int i = 0; i < w.ws_col; i++) {
				if(rocks[i].isActive) {
					if(rocks[i].velocity == 2 && second_time >= 30) {
						init_pair(1, COLOR_RED, COLOR_BLACK);
						attron(COLOR_PAIR(1));
						mvprintw(rocks[i].pos_Y,rocks[i].pos_X,"X");
						attroff(COLOR_PAIR(1));
					}else{
						mvprintw(rocks[i].pos_Y,rocks[i].pos_X,"X");
						rocks[i].velocity = 1;
					}
					rocks[i].pos_Y += rocks[i].velocity;
				}
			}

			if (kbhit()){
				key = getch();
				break;
			}
			for(int i = 0; i <= second_time / 10; i++) {
				rocks[i].isActive = true;
			}
			score += (wtf - oldwtf) * (second_time * 0.75);
			oldwtf = wtf;
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
			refresh();
		}
		if ( key == 'z' ){
			if ( ship_X == 4 ){
				continue;
			} else{
				ship_X = ship_X - 1;
				mvprintw(w.ws_row - 3,ship_X,"/A\\");
			}
			++loops;
		} else if ( key == 'c' ){
			if ( ship_X == w.ws_col - 6 ){
				++loops;
				continue;
			} else{
				ship_X = ship_X + 1;
				mvprintw(w.ws_row - 3,ship_X,"/A\\");
				++loops;
			}
		} else if ( key == 'x'  && !shoot && cooldownShot == 0) {
			cooldownShot = 20;
			shoot = true;
			shoot_X = ship_X;
			shoot_Y = w.ws_row - 4;
			++loops;
		} else {
			continue;
			++loops;
		}
		refresh();
	}
	GOVER:ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE

	std::ifstream highscore;
	highscore.open("highscore");
	int hs;
	highscore >> hs;
	if (score>hs){
		std::ofstream newHS;
		newHS.open("highscore");
		newHS << score;
		newHS.close();
		hs=score;
	}

	refresh();
	clear();
	mvprintw(w.ws_row/2-3,w.ws_col/2-5,"GAME OVER");
	int scoredigits=nDigits(score);
	int hscoredigits=nDigits(hs);
	mvprintw(w.ws_row / 2, w.ws_col / 2 - 3, "Score");
	mvprintw(w.ws_row / 2 + 1, w.ws_col / 2 - scoredigits / 2, "%i", score);
	mvprintw(w.ws_row / 2 + 3, w.ws_col / 2 - 5, "Highscore");
	mvprintw(w.ws_row / 2 + 4, w.ws_col / 2 - hscoredigits / 2, "%i", hs);
	refresh();
	sleep_ms(6000);
	endwin();
	return 0;
}
