/***
 *
 *  Copyright © 2022 Sławek Wernikowski (slawern)
 *  Status icon image for hddled program
 *
 */

#define BACKGROUND_COLOR_IX     1
#define LOWER_ARROW_IX          2
#define UPPER_ARROW_IX          3
#define ICON_PATTERN_ELEMS      (sizeof(ICON_PATTERN) / sizeof(char *))

char *ICON_PATTERN[] = {
/* columns rows colors chars-per-pixel */
"64 128 3 1 ",
NULL,  /* background */
NULL,  /* lower arrow */
NULL,  /* upper arrow */
/* pixels */
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                R                               ",
"                               RRR                              ",
"                              RRRRR                             ",
"                             RRRRRRR                            ",
"                            RRRRRRRRR                           ",
"                           RRRRRRRRRRR                          ",
"                          RRRRRRRRRRRRR                         ",
"                         RRRRRRRRRRRRRRR                        ",
"                        RRRRRRRRRRRRRRRRR                       ",
"                       RRRRRRRRRRRRRRRRRRR                      ",
"                      RRRRRRRRRRRRRRRRRRRRR                     ",
"                     RRRRRRRRRRRRRRRRRRRRRRR                    ",
"                    RRRRRRRRRRRRRRRRRRRRRRRRR                   ",
"                   RRRRRRRRRRRRRRRRRRRRRRRRRRR                  ",
"                  RRRRRRRRRRRRRRRRRRRRRRRRRRRRR                 ",
"                 RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR                ",
"                RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR               ",
"               RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR              ",
"              RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR             ",
"             RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR            ",
"            RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR           ",
"           RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR          ",
"          RRRRRRRRRRRRRRR RRRRRRRRRRRRR RRRRRRRRRRRRRRR         ",
"         RRRRRRRRRRRRRRR  RRRRRRRRRRRRR  RRRRRRRRRRRRRRR        ",
"        RRRRRRRRRRRRRRR   RRRRRRRRRRRRR   RRRRRRRRRRRRRRR       ",
"       RRRRRRRRRRRRRRR    RRRRRRRRRRRRR    RRRRRRRRRRRRRRR      ",
"      RRRRRRRRRRRRRRR     RRRRRRRRRRRRR     RRRRRRRRRRRRRRR     ",
"     RRRRRRRRRRRRRRR      RRRRRRRRRRRRR      RRRRRRRRRRRRRRR    ",
"     RRRRRRRRRRRRRR       RRRRRRRRRRRRR       RRRRRRRRRRRRRR    ",
"     RRRRRRRRRRRRR        RRRRRRRRRRRRR        RRRRRRRRRRRRR    ",
"     RRRRRRRRRRRR         RRRRRRRRRRRRR         RRRRRRRRRRRR    ",
"     RRRRRRRRRRR          RRRRRRRRRRRRR          RRRRRRRRRRR    ",
"     RRRRRRRRRR           RRRRRRRRRRRRR           RRRRRRRRRR    ",
"     RRRRRRRRR            RRRRRRRRRRRRR            RRRRRRRRR    ",
"     RRRRRRRR             RRRRRRRRRRRRR             RRRRRRRR    ",
"     RRRRRRR              RRRRRRRRRRRRR              RRRRRRR    ",
"     RRRRRR               RRRRRRRRRRRRR               RRRRRR    ",
"     RRRRR                RRRRRRRRRRRRR                RRRRR    ",
"     RRRR                 RRRRRRRRRRRRR                 RRRR    ",
"     RRR                  RRRRRRRRRRRRR                  RRR    ",
"     RR                   RRRRRRRRRRRRR                   RR    ",
"     R                    RRRRRRRRRRRRR                    R    ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                          RRRRRRRRRRRRR                         ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"                          WWWWWWWWWWWWW                         ",
"     W                    WWWWWWWWWWWWW                    W    ",
"     WW                   WWWWWWWWWWWWW                   WW    ",
"     WWW                  WWWWWWWWWWWWW                  WWW    ",
"     WWWW                 WWWWWWWWWWWWW                 WWWW    ",
"     WWWWW                WWWWWWWWWWWWW                WWWWW    ",
"     WWWWWW               WWWWWWWWWWWWW               WWWWWW    ",
"     WWWWWWW              WWWWWWWWWWWWW              WWWWWWW    ",
"     WWWWWWWW             WWWWWWWWWWWWW             WWWWWWWW    ",
"     WWWWWWWWW            WWWWWWWWWWWWW            WWWWWWWWW    ",
"     WWWWWWWWWW           WWWWWWWWWWWWW           WWWWWWWWWW    ",
"     WWWWWWWWWWW          WWWWWWWWWWWWW          WWWWWWWWWWW    ",
"     WWWWWWWWWWWW         WWWWWWWWWWWWW         WWWWWWWWWWWW    ",
"     WWWWWWWWWWWWW        WWWWWWWWWWWWW        WWWWWWWWWWWWW    ",
"     WWWWWWWWWWWWWW       WWWWWWWWWWWWW       WWWWWWWWWWWWWW    ",
"     WWWWWWWWWWWWWWW      WWWWWWWWWWWWW      WWWWWWWWWWWWWWW    ",
"      WWWWWWWWWWWWWWW     WWWWWWWWWWWWW     WWWWWWWWWWWWWWW     ",
"       WWWWWWWWWWWWWWW    WWWWWWWWWWWWW    WWWWWWWWWWWWWWW      ",
"        WWWWWWWWWWWWWWW   WWWWWWWWWWWWW   WWWWWWWWWWWWWWW       ",
"         WWWWWWWWWWWWWWW  WWWWWWWWWWWWW  WWWWWWWWWWWWWWW        ",
"          WWWWWWWWWWWWWWW WWWWWWWWWWWWW WWWWWWWWWWWWWWW         ",
"           WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW          ",
"            WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW           ",
"             WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW            ",
"              WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW             ",
"               WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW              ",
"                WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW               ",
"                 WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                ",
"                  WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                 ",
"                   WWWWWWWWWWWWWWWWWWWWWWWWWWW                  ",
"                    WWWWWWWWWWWWWWWWWWWWWWWWW                   ",
"                     WWWWWWWWWWWWWWWWWWWWWWW                    ",
"                      WWWWWWWWWWWWWWWWWWWWW                     ",
"                       WWWWWWWWWWWWWWWWWWW                      ",
"                        WWWWWWWWWWWWWWWWW                       ",
"                         WWWWWWWWWWWWWWW                        ",
"                          WWWWWWWWWWWWW                         ",
"                           WWWWWWWWWWW                          ",
"                            WWWWWWWWW                           ",
"                             WWWWWWW                            ",
"                              WWWWW                             ",
"                               WWW                              ",
"                                W                               ",
"                                                                ",
"                                                                ",
"                                                                ",
"                                                                "
};