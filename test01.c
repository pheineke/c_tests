#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <string.h>

volatile char last_key = 0; // Variable für den letzten Tastendruck
volatile int running = 1;   // Steuerung der Programmausführung

volatile char input[] = "";


void* key_event_listener(void* arg) {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    while (running) {
        last_key = getch();  // Speichert die letzte gedrückte Taste
        if (last_key == '\n') {
            printw("Enter-Taste gedrückt!\n");
        } else if (last_key == 'q') {
            running = 0;     // Beendet das Programm, wenn 'q' gedrückt wird
        } else {
            strcat(input, last_key);
        }
        refresh();
    }

    endwin();
    return NULL;
}

int main() {
    pthread_t thread_id;

    // Startet den Thread, um Tastatureingaben asynchron zu verarbeiten
    pthread_create(&thread_id, NULL, key_event_listener, NULL);

    // Hauptprogrammschleife
    while (running) {
        if (last_key) {  // Überprüft, ob eine Taste außer Enter gedrückt wurde
            if (last_key == '\n') {
                printf("input is: %s\n", input);
            } else {

            }
            last_key = 0;  // Setzt die Variable zurück, um die nächste Eingabe zu erfassen
        }
        //sleep(1);  // Simuliert Arbeit im Hauptprogramm
    }

    // Wartet, bis der Thread beendet ist
    pthread_join(thread_id, NULL);

    printf("Programm beendet.\n");
    return 0;
}
