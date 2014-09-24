#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>
#include <getopt.h>
#include <errno.h>

#include "gui.h"
#include "game.h"
#include "keyboard.h"
#include "common.h"

const char *program_name;
struct game game;

void version();
void usage(const char *);
void draw_greeting();

int main(int argc, char *argv[]) {
  int option;
  int option_index;
  int passes_through_deck = 3;
  int use_utf8 = 0;
  static const struct option options[] = {
    {"help",    no_argument,       NULL, 'h'},
    {"version", no_argument,       NULL, 'v'},
    {"passes",  required_argument, NULL, 'p'},
    {"utf8",    no_argument,       NULL, 'u'}
  };

  program_name = argv[0];

  while ((option = getopt_long(argc, argv, "hvp:u", options, &option_index)) != -1) {
    switch (option) {
    case 'v':
      version();
      exit(0);
    case 'p':
      passes_through_deck = atoi(optarg);
      break;
    case 'u':
      use_utf8 = true;
      break;   
    case 'h':
    default:
      usage(program_name);
      exit(0);
    }
  }

  set_utf8_mode(use_utf8);
  setlocale(LC_ALL, "");
  initscr();
  raw();
  noecho();
  keypad(stdscr, TRUE);
  start_color();
  curs_set(FALSE);
  set_escdelay(0);
  assume_default_colors(COLOR_WHITE, COLOR_GREEN);
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_RED, COLOR_WHITE);
  init_pair(3, COLOR_WHITE, COLOR_BLUE);
  init_pair(4, COLOR_WHITE, COLOR_GREEN);

  int key;

  while (!term_size_ok()) {
    clear();
    mvprintw(1, 1, SMALL_TERM_MSG);
    refresh();
    if ((key = getch()) == 'q' || key == 'Q') {
      endwin();
      return(0);
    }
  }

  clear();
  draw_greeting();
  refresh();

  for (;;) {
    if ((key = getch()) == 'q' || key == 'Q') {
      endwin();
      return(0);
    }
    if (term_size_ok()) {
      clear();
      draw_greeting();
      refresh();
      if (key == KEY_SPACEBAR) {
        clear();
        refresh();
        game_init(&game, passes_through_deck);
        break;
      }
    } else if (key == KEY_RESIZE) {
      clear();
      mvprintw(1, 1, SMALL_TERM_MSG);
      refresh();
    }
  }

  do {
    keyboard_event(getch());
  } while (!game_won());

  endwin();
  game_end();
  printf("You won.\n");

  return(0);
}

void draw_greeting() {
  mvprintw(8, 26, "Welcome to tty-solitaire.");
  mvprintw(10, 23, "Move with the arrow keys or hjkl.");
  mvprintw(11, 19, "Use the space bar to mark and move cards.");
  mvprintw(12, 16, "After marking a card you can use m to increase ");
  mvprintw(13, 17, "and n to decrease the number of marked cards.");
  mvprintw(15, 19, "Press the space bar to play or q to quit.");
}

void usage(const char *program_name) {
  printf("usage: %s [-v|--version] [-h|--help] [-p|--passes=NUMBER] [-u|--utf8]\n", program_name);
  printf("  -v, --version  Show version\n");
  printf("  -h, --help     Show this message\n");
  printf("  -p, --passes   Number of passes through the deck\n");
  printf("  -u, --utf8     Use UTF8 symbols for suits instead of ascii\n");
}

void version() {
  FILE *version_file;
  char version_string[6];

  if (!(version_file = fopen("VERSION", "rb"))) {
    tty_solitaire_generic_error(errno, __FILE__, __LINE__);
  }
  fread(version_string, 1, 5, version_file);
  version_string[5] = '\0';
  printf("%s\n", version_string);
  fclose(version_file);
}


// vim: shiftwidth=2 tabstop=2 softtabstop=2

