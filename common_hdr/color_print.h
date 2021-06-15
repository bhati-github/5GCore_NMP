#ifndef COLOR_PRINT_H
#define COLOR_PRINT_H

#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define CYAN_PRINT(fmt,args...) \
do { \
        printf(ANSI_COLOR_CYAN  fmt,##args); printf(ANSI_COLOR_RESET); \
} while(0);


#define GREEN_PRINT(fmt,args...) \
do { \
    printf(ANSI_COLOR_GREEN  fmt,##args); printf(ANSI_COLOR_RESET); \
} while(0);


#define MAGENTA_PRINT(fmt,args...) \
do { \
    printf(ANSI_COLOR_MAGENTA  fmt,##args); printf(ANSI_COLOR_RESET); \
} while(0);


#define RED_PRINT(fmt,args...) \
do { \
    printf(ANSI_COLOR_RED fmt,##args); printf(ANSI_COLOR_RESET); \
} while(0);


#define YELLOW_PRINT(fmt,args...) \
do { \
    printf(ANSI_COLOR_YELLOW fmt,##args); printf(ANSI_COLOR_RESET); \
} while(0);


#endif 
