#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h> // Für usleep()

volatile int running = 1;   // Steuerung der Programmausführung

#define MAX_INPUT_SIZE 256   // Maximale Eingabelänge
char input[MAX_INPUT_SIZE] = ""; // Eingabepuffer

void* key_event_listener(void* arg) {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr, TRUE);   // Setzt getch() in den Non-Blocking-Modus

    while (running) {
        int ch = getch();  // Versucht, ein Zeichen zu lesen, ohne zu blockieren
        if (ch != ERR) {   // Wenn ein Zeichen verfügbar ist
            if (ch == '\n') {
                // Zeichenfolge wird nach dem Drücken von Enter geprüft
                printw("\ninput is: %s\n", input);
                refresh();  // Aktualisiert den Bildschirm
                if (strcmp(input, "quit") == 0) {  // Vergleicht den Inhalt von `input` mit "quit"
                    printw("HIER\n");
                    refresh();
                    running = 0;
                }
                // Zurücksetzen des Puffers
                memset(input, 0, sizeof(input)); // Leert den gesamten Puffer
            } else {
                // Fügt Zeichen zum Puffer hinzu, wenn es nicht Enter ist
                int len = strlen(input);
                if (len < MAX_INPUT_SIZE - 1) {  // Überprüft, ob genügend Platz im Puffer vorhanden ist
                    input[len] = (char)ch;
                    input[len + 1] = '\0';  // Füge das Null-Terminatorzeichen hinzu
                    printw("%c", (char)ch);  // Zeigt das Zeichen auf dem Bildschirm an
                    refresh();               // Aktualisiert den Bildschirm
                }
            }
        }
        usleep(10000);  // Kleine Pause, um CPU-Last zu reduzieren
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
        // Die Hauptschleife wartet einfach darauf, dass der Thread beendet wird
        usleep(10000);  // Kleine Pause, um CPU-Last zu reduzieren
    }

    // Wartet, bis der Thread beendet ist
    pthread_join(thread_id, NULL);

    printf("Programm beendet.\n");
    return 0;
}
