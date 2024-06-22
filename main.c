#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        printf("Error: could not open file %s\n", argv[1]);
        return 1;
    }

    fseek(fptr, 0L, SEEK_END);
    size_t size = ftell(fptr);
    fseek(fptr,0L,SEEK_SET);

    char *buffer = malloc(size);

    if (buffer == NULL) {
        printf("Error: could not allocate memory\n");
        return 1;
    }

    if (fread(buffer, sizeof(char), size, fptr) != size) {
        printf("Error: could not read file\n");
        free(buffer);
        fclose(fptr);
        return 1;
    }

    fclose(fptr);

    size_t linecount = 1;

    for (size_t i = 0; i < size; i++) {
        if (buffer[i] == '\n') {
            linecount++;
        }
    }

    char **lines = malloc(linecount*sizeof(char*));
    size_t *linelengths = malloc(linecount*sizeof(size_t));

    if (lines == NULL) {
        printf("Error: could not allocate memory\n");
        return 1;
    }

    // funky loops
    size_t i = 0;
    size_t index = 0;
    while (index < size) {
        size_t start = index;
        while (index < size && buffer[index] != '\n') {
            index++;
        }
        size_t length = index - start;
        lines[i] = malloc(length + 1);
        if (lines[i] == NULL) {
            printf("Error: could not allocate memory\n");
            return 1;
        }
        strncpy(lines[i], buffer + start, length);
        lines[i][length] = '\0';
        linelengths[i] = length;
        index++;
        i++;

        // If the last character was a newline and we're at the end of the buffer, add an empty line
        if (index == size && buffer[index - 1] == '\n') {
            lines[i] = malloc(1);
            if (lines[i] == NULL) {
                printf("Error: could not allocate memory\n");
                return 1;
            }
            lines[i][0] = '\0';
            linelengths[i] = 0;
            i++;
        }
    }

    free(buffer);

    WINDOW* window = initscr();

    size_t x = 0;
    size_t y = 0;

    char* commandcache = malloc(256*sizeof(char));
    size_t commandi = 0;
    size_t commandsize = 256;

    char* commandtitle = "Pencil Command: ";
    size_t commandtitlesize = strlen(commandtitle);

    bool oldkeypadness = is_keypad;
    keypad(stdscr, TRUE);
    while (true) {
        int width = getmaxx(window);
        int height = getmaxy(window);

        if (height < 3) {
            clear();
            endwin();
            printf("Error: terminal too small\n");
            return 1;
        }

        int center = (height-2)/2;

        signed long offset = y-center;

        if (offset < 0) {
            offset = 0;
        }

        for (size_t i = 0; i < height-2 && i < linecount; i++) {
            size_t ypos = i+offset;
            mvaddstr(i, 0, (ypos >= 0 && ypos < linecount) ? lines[ypos] : "");
        }

        mvaddstr(height-1, 0, commandtitle);
        mvaddstr(height-1, commandtitlesize, commandcache);

        refresh();

        move(y - offset, x);

        int character = getch();

        // add to command cache
        if (character == '\n') {
            // do something with command
            if (commandi > 0) {
                bool justadd = false;
                if (commandcache[0] == '/') {
                    if (commandcache[1] == 'q') {
                        break;
                    } else if (commandcache[1] == 's') {
                        // save file
                        if (argc < 2) {
                            justadd = true;
                        } else {
                            FILE *fptr = fopen(argv[1], "w");
                            if (fptr == NULL) {
                                justadd = true;
                            } else {
                                for (size_t i = 0; i < linecount; i++) {
                                    fprintf(fptr, (i == linecount-1) ? "%s" : "%s\n", lines[i]);
                                }
                                fclose(fptr);
                            }
                        }
                    } else if (commandcache[1] == 'b') {
                        // check if the rest of the command is a number
                        bool isnumber = true;
                        for (size_t i = 2; i < commandi; i++) {
                            if (commandcache[i] < '0' || commandcache[i] > '9') {
                                isnumber = false;
                                break;
                            }
                        }

                        if (commandi == 2) {
                            isnumber = false;
                        }

                        size_t number = 0;
                        if (isnumber) {
                            for (size_t i = 2; i < commandi; i++) {
                                number = number*10 + (commandcache[i] - '0');
                            }
                        } else {
                            number = 1;
                        }


                        // backspace number of characters
                        // first, remove characters to the left of cursor
                        // then, move characters to the right of cursor to the left
                        // then update linelengths
                        // then move cursor
                        // REMEMBER TO CARE ABOUT NEWLINES!!!
                        for (size_t po = 0; po < number; po++) {
                            if (x > 0) {
                                // remove char
                                size_t curline = y;
                                size_t oldsize = linelengths[curline];
                                size_t newsize = oldsize - 1;
                                for (size_t k = x; k < oldsize; k++) {
                                    lines[curline][k-1] = lines[curline][k];
                                }
                                lines[curline] = realloc(lines[curline], newsize + 1);
                                if (lines[curline] == NULL) {
                                    clear();
                                    endwin();
                                    printf("Error: could not allocate memory\n");
                                    return 1;
                                }
                                linelengths[curline] = newsize;
                                lines[curline][newsize] = '\0';

                                x--;
                            } else {
                                if (y > 0) {
                                    // remove newline
                                    size_t curline = y;
                                    size_t prevline = y-1;

                                    size_t cursize = linelengths[curline];
                                    size_t prevsize = linelengths[prevline];

                                    size_t newsize = cursize + prevsize;
                                    lines[prevline] = realloc(lines[prevline], newsize + 1);
                                    if (lines[prevline] == NULL) {
                                        clear();
                                        endwin();
                                        printf("Error: could not allocate memory\n");
                                        return 1;
                                    }

                                    for (size_t k = 0; k < cursize; k++) {
                                        lines[prevline][prevsize+k] = lines[curline][k];
                                    }

                                    lines[prevline][newsize] = '\0';
                                    linelengths[prevline] = newsize;
                                    
                                    free(lines[curline]);

                                    for (size_t k = curline; k < linecount-1; k++) {
                                        lines[k] = lines[k+1];
                                        linelengths[k] = linelengths[k+1];
                                    }

                                    linecount--;

                                    lines = realloc(lines, linecount*sizeof(char*));
                                    linelengths = realloc(linelengths, linecount*sizeof(size_t));

                                    if (lines == NULL || linelengths == NULL) {
                                        clear();
                                        endwin();
                                        printf("Error: could not allocate memory\n");
                                        return 1;
                                    }

                                    y--;
                                    x = prevsize;
                                }
                            }
                        }
                    } else if (commandcache[1] == 'n') {
                        // new line woohoo

                        size_t curline = y;

                        linecount++;

                        lines = realloc(lines, linecount*sizeof(char*));
                        linelengths = realloc(linelengths, linecount*sizeof(size_t));

                        if (lines == NULL || linelengths == NULL) {
                            clear();
                            endwin();
                            printf("Error: could not allocate memory\n");
                            return 1;
                        }

                        for (size_t j = linecount-1; j > curline+1; j--) {
                            lines[j] = lines[j-1];
                            linelengths[j] = linelengths[j-1];
                        }

                        y++;

                        lines[y] = malloc(1*sizeof(char));
                        lines[y][0] = '\0';

                        // copy all of the characters to the right of the cursor to the new line

                        size_t addedsize = linelengths[curline] - x;
                        lines[y] = realloc(lines[y], addedsize + 1);
                        if (lines[y] == NULL) {
                            clear();
                            endwin();
                            printf("Error: could not allocate memory\n");
                            return 1;
                        }

                        for (size_t j = 0; j < addedsize; j++) {
                            lines[y][j] = lines[curline][x+j];
                        }
                        
                        lines[y][addedsize] = '\0';

                        linelengths[y] = addedsize;

                        lines[curline] = realloc(lines[curline], x + 1);
                        if (lines[curline] == NULL) {
                            clear();
                            endwin();
                            printf("Error: could not allocate memory\n");
                            return 1;
                        }

                        linelengths[curline] = x;

                        lines[curline][x] = '\0';

                        x = 0;
                    } else if (commandcache[1] == 'w') {
                        bool isnumber = true;
                        for (size_t i = 2; i < commandi; i++) {
                            if (commandcache[i] < '0' || commandcache[i] > '9') {
                                isnumber = false;
                                break;
                            }
                        }

                        if (commandi == 2) {
                            isnumber = false;
                        }

                        size_t number = 0;
                        if (isnumber) {
                            for (size_t i = 2; i < commandi; i++) {
                                number = number*10 + (commandcache[i] - '0');
                            }
                        } else {
                            number = 1;
                        }

                        size_t curline = y;
                        size_t oldsize = linelengths[curline];
                        size_t newsize = oldsize + number;
                        lines[curline] = realloc(lines[curline], newsize + 1);
                        if (lines[curline] == NULL) {
                            clear();
                            endwin();
                            printf("Error: could not allocate memory\n");
                            return 1;
                        }
                        
                        for (size_t i = oldsize; i > x; i--) {
                            lines[curline][i+number-1] = lines[curline][i-1];
                        }

                        for (size_t i = 0; i < number; i++) {
                            lines[curline][x+i] = ' ';
                        }

                        linelengths[curline] = newsize;

                        lines[curline][newsize] = '\0';

                        x += number;
                    } else if (commandcache[1] == 't') {
                        bool isnumber = true;
                        for (size_t i = 2; i < commandi; i++) {
                            if (commandcache[i] < '0' || commandcache[i] > '9') {
                                isnumber = false;
                                break;
                            }
                        }

                        if (commandi == 2) {
                            isnumber = false;
                        }

                        size_t number = 0;
                        if (isnumber) {
                            for (size_t i = 2; i < commandi; i++) {
                                number = number*10 + (commandcache[i] - '0');
                            }
                        } else {
                            number = 1;
                        }

                        number *= 4; // tab is just 4 spaces lol

                        size_t curline = y;
                        size_t oldsize = linelengths[curline];
                        size_t newsize = oldsize + number;
                        lines[curline] = realloc(lines[curline], newsize + 1);
                        if (lines[curline] == NULL) {
                            clear();
                            endwin();
                            printf("Error: could not allocate memory\n");
                            return 1;
                        }
                        
                        for (size_t i = oldsize; i > x; i--) {
                            lines[curline][i+number-1] = lines[curline][i-1];
                        }

                        for (size_t i = 0; i < number; i++) {
                            lines[curline][x+i] = ' ';
                        }

                        linelengths[curline] = newsize;

                        lines[curline][newsize] = '\0';

                        x += number;
                    } else {
                        justadd = true;
                    }
                } else {
                    justadd = true;
                }

                if (justadd) {
                    size_t curline = y;
                    size_t oldsize = linelengths[curline];
                    size_t newsize = oldsize + commandi;
                    lines[curline] = realloc(lines[curline], newsize + 1);
                    if (lines[curline] == NULL) {
                        clear();
                        endwin();
                        printf("Error: could not allocate memory\n");
                        return 1;
                    }
                    
                    for (size_t i = oldsize; i > x; i--) {
                        lines[curline][i+commandi-1] = lines[curline][i-1];
                    }

                    for (size_t i = 0; i < commandi; i++) {
                        lines[curline][x+i] = commandcache[i];
                    }

                    linelengths[curline] = newsize;

                    lines[curline][newsize] = '\0';

                    x += commandi;
                }
            }

            for (size_t i = 0; i < commandi; i++) {
                commandcache[i] = '\0';
            }
            commandi = 0;
        } else if (character == KEY_BACKSPACE) {
            if (commandi > 0) {
                commandi--;
                commandcache[commandi] = '\0';
            }
        } else if (character == KEY_LEFT) {
            if (x > 0) {
                x--;
            } else if (x == 0) {
                if (y > 0) {
                    y--;
                    x = linelengths[y];
                }
            }
        } else if (character == KEY_RIGHT) {
            if (x < linelengths[y]) {
                x++;
            } else if (x == linelengths[y]) {
                if (y < linecount-1) {
                    y++;
                    x = 0;
                }
            }
        } else if (character == KEY_UP) {
            if (y > 0) {
                y--;
                if (x > linelengths[y]) {
                    x = linelengths[y];
                }
            }
        } else if (character == KEY_DOWN) {
            if (y < linecount-1) {
                y++;
                if (x > linelengths[y]) {
                    x = linelengths[y];
                }
            }
        } else if (character != '\n' && character != 127) {
            if (character == '\t') {
                if (commandi < commandsize-3) {
                    for (char i = 0; i < 4; i++) {
                        commandcache[commandi] = ' ';
                        commandi++;
                    }
                }
            } else if (commandi < commandsize) {
                commandcache[commandi] = character;
                commandi++;
            }
        }

        clear();
    }

    keypad(stdscr, oldkeypadness);

    endwin();

    for (size_t i = 0; i < linecount; i++) {
        free(lines[i]);
    }
    free(lines);

    return 0;
}