/*
 * Generated by: ./script/extract-boxes /Users/lvillani/Dropbox/DevelHack-Rasky/reference/player/bounding boxes/ -o src/hitboxes.gen.c -p dummy-
 * On: 2016-07-14 17:05:00.124854
 */

#include "hitbox.h"

/* Box order is: white, blue, red */

const HitboxDesc Hitboxes[MAX_HITBOXES] = {
    { "dummy-block", {
        /* 1 */ { WBOX(29,24, 31,93),   BBOX(29,26, 31,91),   NO_BOX(),             },
        /* 2 */ { WBOX(29,30, 34,87),   NO_BOX(),             NO_BOX(),             },
        /* 3 */ { WBOX(28,25, 32,94),   BBOX(28,25, 32,94),   NO_BOX(),             },
    }},
    { "dummy-fall", {
        /* 1 */ { WBOX(19,23, 39,86),   NO_BOX(),             NO_BOX(),             },
        /* 2 */ { WBOX(12,29, 71,75),   NO_BOX(),             NO_BOX(),             },
        /* 3 */ { WBOX(10,68, 78,49),   NO_BOX(),             NO_BOX(),             },
        /* 4 */ { WBOX(9,99, 101,22),   NO_BOX(),             NO_BOX(),             },
    }},
    { "dummy-hit", {
        /* 1 */ { WBOX(17,32, 30,84),   NO_BOX(),             NO_BOX(),             },
        /* 2 */ { WBOX(23,32, 32,84),   NO_BOX(),             NO_BOX(),             },
    }},
    { "dummy-idle", {
        /* 1 */ { WBOX(30,22, 31,95),   BBOX(28,23, 35,95),   NO_BOX(),             },
        /* 2 */ { WBOX(30,26, 29,92),   BBOX(26,30, 36,85),   NO_BOX(),             },
        /* 3 */ { WBOX(30,28, 29,91),   BBOX(26,33, 37,83),   NO_BOX(),             },
        /* 4 */ { WBOX(32,29, 30,91),   BBOX(29,34, 36,82),   NO_BOX(),             },
        /* 5 */ { WBOX(30,29, 34,89),   BBOX(28,30, 38,87),   NO_BOX(),             },
    }},
    { "dummy-jump", {
        /* 1 */ { WBOX(30,26, 30,94),   BBOX(28,30, 35,77),   NO_BOX(),             },
        /* 2 */ { WBOX(31,31, 30,87),   BBOX(27,36, 36,71),   NO_BOX(),             },
        /* 3 */ { WBOX(30,13, 31,96),   BBOX(27,20, 36,72),   NO_BOX(),             },
        /* 4 */ { WBOX(28,8, 30,88),    BBOX(25,15, 36,68),   NO_BOX(),             },
        /* 5 */ { WBOX(28,23, 31,82),   BBOX(25,29, 37,65),   NO_BOX(),             },
        /* 6 */ { WBOX(30,35, 32,80),   BBOX(27,42, 37,66),   NO_BOX(),             },
    }},
    { "dummy-jumpkick", {
        /* 1 */ { WBOX(16,32, 30,86),   BBOX(12,38, 38,70),   NO_BOX(),             },
        /* 2 */ { WBOX(25,11, 29,76),   BBOX(20,18, 38,65),   RBOX(71,75, 22,12),   },
        /* 3 */ { WBOX(33,16, 28,72),   BBOX(29,24, 30,62),   RBOX(76,80, 12,19),   },
    }},
    { "dummy-jumppunch", {
        /* 1 */ { WBOX(33,29, 24,90),   BBOX(28,33, 33,73),   NO_BOX(),             },
        /* 2 */ { WBOX(39,12, 41,88),   BBOX(41,13, 32,72),   RBOX(80,66, 25,23),   },
        /* 3 */ { WBOX(36,18, 39,83),   BBOX(37,19, 35,71),   RBOX(76,79, 9,11),    },
    }},
    { "dummy-kick", {
        /* 1 */ { WBOX(43,26, 25,90),   BBOX(38,31, 28,69),   NO_BOX(),             },
        /* 2 */ { WBOX(52,42, 19,74),   BBOX(48,46, 25,45),   NO_BOX(),             },
        /* 3 */ { WBOX(43,23, 40,93),   BBOX(39,25, 32,62),   RBOX(95,42, 32,23),   },
        /* 4 */ { WBOX(44,36, 30,79),   BBOX(41,37, 34,54),   NO_BOX(),             },
        /* 5 */ { WBOX(49,28, 29,90),   BBOX(41,27, 34,64),   NO_BOX(),             },
    }},
    { "dummy-punch", {
        /* 1 */ { WBOX(35,28, 37,91),   BBOX(34,31, 29,86),   NO_BOX(),             },
        /* 2 */ { WBOX(27,25, 37,88),   BBOX(27,24, 35,92),   RBOX(68,63, 10,11),   },
        /* 3 */ { WBOX(23,25, 35,91),   BBOX(22,24, 36,91),   RBOX(72,54, 50,13),   },
    }},
    { "dummy-special", {
        /* 1 */ { WBOX(23,23, 35,93),   BBOX(19,25, 39,90),   NO_BOX(),             },
        /* 2 */ { WBOX(22,26, 32,89),   BBOX(19,29, 36,87),   NO_BOX(),             },
        /* 3 */ { WBOX(37,25, 32,94),   NO_BOX(),             NO_BOX(),             },
        /* 4 */ { WBOX(40,24, 33,92),   NO_BOX(),             RBOX(84,59, 14,28),   },
    }},
    { "dummy-special2", {
        /* 1 */ { WBOX(31,34, 27,87),   BBOX(28,37, 32,73),   NO_BOX(),             },
        /* 2 */ { WBOX(31,18, 26,76),   BBOX(27,23, 34,67),   NO_BOX(),             },
        /* 3 */ { WBOX(21,36, 60,51),   NO_BOX(),             RBOX(88,37, 7,28),    },
        /* 4 */ { WBOX(45,36, 41,71),   NO_BOX(),             RBOX(93,12, 9,50),    },
        /* 5 */ { WBOX(47,50, 50,54),   NO_BOX(),             NO_BOX(),             },
    }},
    { "dummy-walk", {
        /* 1 */ { WBOX(29,24, 34,92),   BBOX(29,24, 33,93),   NO_BOX(),             },
        /* 2 */ { WBOX(32,27, 30,89),   BBOX(32,27, 31,89),   NO_BOX(),             },
        /* 3 */ { WBOX(34,29, 31,86),   BBOX(34,29, 31,86),   NO_BOX(),             },
        /* 4 */ { WBOX(35,28, 33,85),   BBOX(35,29, 33,84),   NO_BOX(),             },
        /* 5 */ { WBOX(36,28, 34,87),   BBOX(36,28, 34,87),   NO_BOX(),             },
        /* 6 */ { WBOX(36,28, 32,84),   BBOX(36,28, 32,84),   NO_BOX(),             },
        /* 7 */ { WBOX(34,30, 33,82),   BBOX(34,32, 33,80),   NO_BOX(),             },
        /* 8 */ { WBOX(32,29, 33,86),   BBOX(32,31, 33,84),   NO_BOX(),             },
    }},
};
