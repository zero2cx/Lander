#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

struct rock_t {
    int id;
    int pos_X = -1;
    int pos_Y = 0;
    bool isActive;
    bool needsRock;
} rocks[0];

void destroyRock(int id) {
    rocks[id].pos_Y = 0;
    rocks[id].pos_X = -1;
    rocks[id].isActive = false;
}

void createRock(int id) {
    rock_t r;
    r.id = id;
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


int main(){
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

    for(int i = 0; i < w.ws_col; i++) {
        createRock(i);
        srand((time(0) * i) + time(0));
        rocks[i].pos_X = rand()%(w.ws_col - 7)+4;
        rocks[i].isActive = false;
    }
    rocks[0].isActive = true;
	nodelay(stdscr, TRUE);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

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
                if (rocks[i].pos_Y > w.ws_row ){
                    wtf++;
                    destroyRock(i);
                    rocks[i].needsRock = 1;
                }
                if ((rocks[i].pos_Y == shoot_Y || rocks[i].pos_Y == shoot_Y + 1 || rocks[i].pos_Y == shoot_Y - 1) && (rocks[i].pos_X == shoot_X ||
                                                                                                                     rocks[i].pos_X == shoot_X + 1 ||
                                                                                                                     rocks[i].pos_X == shoot_X - 1)) {
                    wtf += 2;
                    rocks[i].needsRock = 1;
                    shoot = false;
                    shoot_X = -1;
                    shoot_Y = -1;
                }
                if (rocks[i].needsRock == 1 ) {
                    srand((time(0) * i) + time(0));
                    rocks[i].pos_X = rand()%(w.ws_col - 7)+4;
                    rocks[i].needsRock = 0;
                }
                if ((ship_X == rocks[i].pos_X || ship_X + 1 == rocks[i].pos_X || ship_X + 2 == rocks[i].pos_X) && (rocks[i].pos_Y == w.ws_row - 3) ){
                    goto GOVER;
                }
            }

			if (kbhit()){
				key = getch();
				break;
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
					shoot_Y = -1;
				}
			}
			refresh();
			mvprintw(w.ws_row - 3,ship_X,"/A\\");
            for(int i = 0; i < w.ws_col; i++) {
                if(rocks[i].isActive) {
                    mvprintw(rocks[i].pos_Y,rocks[i].pos_X,"X");
                    rocks[i].pos_Y++;
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
		} else if ( key == 'c' ){
			if ( ship_X == w.ws_col - 6 ){
				continue;
			} else{
				ship_X = ship_X + 1;
				mvprintw(w.ws_row - 3,ship_X,"/A\\");
			}
		} else if ( key == 'x'  && !shoot) {
            shoot = true;
            shoot_X = ship_X;
            shoot_Y = w.ws_row - 4;;
        }else if(key == 'p') {
            destroyRock(0);
		} else {
			continue;
		}
		refresh();
	}
	GOVER:ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);   // GET THE TERMINAL SIZE
	refresh();
	clear();
	mvprintw(w.ws_row/2,w.ws_col/2-4,"GAME OVER");
	refresh();
	sleep_ms(3000);
	endwin();
	return 0;
}
