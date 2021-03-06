#include "frames.h"
#include "specials.h"

#define NEXT(n) ((n)+1)
#define MOVE(x,y) (x),(y)
#define SPEED(n) (16-(n))
#define DAMAGE(n) (n)

const AnimDesc Rasky = {
    { // frames
        { 0,  0,        MOVE(0,0), "rasky-idle", 0, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        { 1,  0,        MOVE(0,0), "rasky-idle", 1, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        { 2,  0,        MOVE(0,0), "rasky-idle", 2, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        { 3,  0,        MOVE(0,0), "rasky-idle", 3, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        { 4,  0,        MOVE(0,0), "rasky-idle", 4, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        { 5,  0,        MOVE(0,0), "rasky-idle", 5, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        { 6,  0,        MOVE(0,0), "rasky-idle", 6, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        { 7,  NEXT(0),  MOVE(0,0), "rasky-idle", 7, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
        /******************************************************************/
        { 8,  0,        MOVE(7,0), "rasky-walk", 0, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
        { 9,  0,        MOVE(7,0), "rasky-walk", 1, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
        { 10, 0,        MOVE(7,0), "rasky-walk", 2, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
        { 11, 0,        MOVE(7,0), "rasky-walk", 3, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
        { 12, 0,        MOVE(7,0), "rasky-walk", 4, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
        { 13, NEXT(8),  MOVE(7,0), "rasky-walk", 5, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
        /******************************************************************/
        { 14, 0,        MOVE(-7,0), "rasky-walk", 5, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
        { 15, 0,        MOVE(-7,0), "rasky-walk", 4, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
        { 16, 0,        MOVE(-7,0), "rasky-walk", 3, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
        { 17, 0,        MOVE(-7,0), "rasky-walk", 2, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
        { 18, 0,        MOVE(-7,0), "rasky-walk", 1, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
        { 19, NEXT(14), MOVE(-7,0), "rasky-walk", 0, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
        /******************************************************************/
        { 20, 0,        MOVE(8,-15), "rasky-idle", 0, 8, SPEED(8), FST_JMP},
        { 21, 0,        MOVE(8,-10), "rasky-idle", 0, 8, SPEED(8), FST_JMP},
        { 22, 0,        MOVE(8,-5),  "rasky-idle", 0, 8, SPEED(8), FST_JMP},
        { 23, 0,        MOVE(8,0),   "rasky-idle", 0, 8, SPEED(8), FST_JMP},
        { 24, 0,        MOVE(8,5),   "rasky-idle", 0, 8, SPEED(8), FST_JMP},
        { 25, 0,        MOVE(8,10),  "rasky-idle", 0, 8, SPEED(8), FST_JMP},
        { 26, NEXT(0),  MOVE(8,15),  "rasky-idle", 0, 8, SPEED(8), FST_JMP},
        { 0, 0, },
    },
    { // keyframes
        /* idle */ 0,
        /* fwd  */ 8,
        /* bwd  */ 14,
        /* jump */ 20,
    },
    /* pivot */ 64, 122,
    /* palette */ "rasky-idle.pal.bin",
};

#define BASE_WBOX WBOX(39,10, 50,110)
#define BASE_BBOX BBOX(39,10, 50,110)
#define BASE_RBOX RBOX(39,10, 50,110)

const AnimDesc Dummy = {
    { // frames
        { 0,  0,        MOVE(0,0), "dummy-idle", 0, 5, SPEED(10),  FST_IDL|FCWALK|FCJMP,        DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 1,  0,        MOVE(0,0), "dummy-idle", 1, 5, SPEED(10),  FST_IDL|FCWALK|FCJMP,        DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 2,  0,        MOVE(0,0), "dummy-idle", 2, 5, SPEED(10),  FST_IDL|FCWALK|FCJMP,        DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 3,  0,        MOVE(0,0), "dummy-idle", 3, 5, SPEED(10),  FST_IDL|FCWALK|FCJMP,        DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 4,  NEXT(0),  MOVE(0,0), "dummy-idle", 4, 5, SPEED(10),  FST_IDL|FCWALK|FCJMP,        DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        /*********************************************************************************************************************************/
        { 5,  0,        MOVE(7,0), "dummy-walk", 0, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 6,  0,        MOVE(7,0), "dummy-walk", 1, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 7, 0,         MOVE(7,0), "dummy-walk", 2, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 8, 0,         MOVE(7,0), "dummy-walk", 3, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 9, 0,         MOVE(7,0), "dummy-walk", 4, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 10, 0,        MOVE(7,0), "dummy-walk", 5, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 11, 0,        MOVE(7,0), "dummy-walk", 6, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 12, NEXT(5),  MOVE(7,0), "dummy-walk", 7, 8, SPEED(8),   FST_FWD|FCIDLE|FCBWD|FCJMP,  DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        /*********************************************************************************************************************************/
        { 13, 0,        MOVE(-7,0), "dummy-walk", 7, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 14, 0,        MOVE(-7,0), "dummy-walk", 6, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 15, 0,        MOVE(-7,0), "dummy-walk", 5, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 16, 0,        MOVE(-7,0), "dummy-walk", 4, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 17, 0,        MOVE(-7,0), "dummy-walk", 3, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 18, 0,        MOVE(-7,0), "dummy-walk", 2, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 19, 0,        MOVE(-7,0), "dummy-walk", 1, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 20, NEXT(13), MOVE(-7,0), "dummy-walk", 0, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP, DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        /*********************************************************************************************************************************/
        { 21, 0,        MOVE(8,-15), "dummy-jump", 0, 6, SPEED(8),  FST_JMP,                    DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 22, 0,        MOVE(8,-10), "dummy-jump", 1, 6, SPEED(8),  FST_JMP,                    DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 23, 0,        MOVE(8,-5),  "dummy-jump", 2, 6, SPEED(8),  FST_JMP,                    DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 24, 0,        MOVE(0,0),   "dummy-jump", 3, 6, 1,         FST_JMP|FCJMPCH,            DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 25, 0,        MOVE(8,5),   "dummy-jump", 3, 6, SPEED(8),  FST_JMP,                    DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 26, 0,        MOVE(8,10),  "dummy-jump", 4, 6, SPEED(8),  FST_JMP,                    DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        { 27, NEXT(0),  MOVE(8,15),  "dummy-jump", 5, 6, SPEED(8),  FST_JMP,                    DAMAGE(0), {BASE_WBOX, BASE_BBOX}},
        /*********************************************************************************************************************************/
        { 28,  0,       MOVE(0,0), "dummy-punch", 0, 3, SPEED(14),  0,                          DAMAGE(3), {BASE_WBOX, BASE_BBOX}},
        { 29,  0,       MOVE(0,0), "dummy-punch", 1, 3, SPEED(14),  0,                          DAMAGE(3), {BASE_WBOX, BASE_RBOX}},
        { 30,  0,       MOVE(0,0), "dummy-punch", 2, 3, SPEED(14),  0,                          DAMAGE(3), {BASE_WBOX, BASE_RBOX}},
        { 31,  0,       MOVE(0,0), "dummy-punch", 1, 3, SPEED(14),  0,                          DAMAGE(3), {BASE_WBOX, BASE_RBOX}},
        { 32,  NEXT(0), MOVE(0,0), "dummy-punch", 0, 3, SPEED(14),  0,                          DAMAGE(3), {BASE_WBOX, BASE_BBOX}},
        /*********************************************************************************************************************************/
        { 33,  0,       MOVE(0,0), "dummy-kick", 0, 5, SPEED(12),   0,                          DAMAGE(5), {BASE_WBOX, BASE_BBOX}},
        { 34,  0,       MOVE(0,0), "dummy-kick", 1, 5, SPEED(12),   0,                          DAMAGE(5), {BASE_WBOX, BASE_RBOX}},
        { 35,  0,       MOVE(0,0), "dummy-kick", 2, 5, SPEED(12),   0,                          DAMAGE(5), {BASE_WBOX, BASE_RBOX}},
        { 36,  0,       MOVE(0,0), "dummy-kick", 3, 5, SPEED(12),   0,                          DAMAGE(5), {BASE_WBOX, BASE_RBOX}},
        { 37,  NEXT(0), MOVE(0,0), "dummy-kick", 4, 5, SPEED(12),   0,                          DAMAGE(5), {BASE_WBOX, BASE_BBOX}},
        /*********************************************************************************************************************************/
        { 38,  0,       MOVE(0,0), "dummy-hit", 0, 2, SPEED(10),   0,                           DAMAGE(0), {BASE_WBOX }},
        { 39,  0,       MOVE(0,0), "dummy-hit", 1, 2, SPEED(10),   0,                           DAMAGE(0), {BASE_WBOX }},
        { 40,  NEXT(0), MOVE(0,0), "dummy-hit", 0, 2, SPEED(10),   0,                           DAMAGE(0), {BASE_WBOX }},
        /*********************************************************************************************************************************/
        { 41,  0,       MOVE(0,0), "dummy-special", 0, 4, SPEED(10),   0,                       DAMAGE(0), {BASE_WBOX }},
        { 42,  0,       MOVE(0,0), "dummy-special", 1, 4, SPEED(10),   0,                       DAMAGE(0), {BASE_WBOX }},
        { 43,  0,       MOVE(0,0), "dummy-special", 2, 4, SPEED(10),   0,                       DAMAGE(0), {BASE_WBOX }},
        { 44,  0,       MOVE(0,0), "dummy-special", 3, 4, SPEED(0),    FSPECIAL,                DAMAGE(0), {BASE_WBOX }},
        { 45,  0,       MOVE(0,0), "dummy-special", 3, 4, SPEED(0),    0,                       DAMAGE(0), {BASE_WBOX }},
        { 46,  0,       MOVE(0,0), "dummy-special", 2, 4, SPEED(12),   0,                       DAMAGE(0), {BASE_WBOX }},
        { 47,  0,       MOVE(0,0), "dummy-special", 1, 4, SPEED(12),   0,                       DAMAGE(0), {BASE_WBOX }},
        { 48,  NEXT(0), MOVE(0,0), "dummy-special", 0, 4, SPEED(12),   0,                       DAMAGE(0), {BASE_WBOX }},
        /*********************************************************************************************************************************/
        { 49,  0,       MOVE(-8,0), "dummy-fall", 0, 4, SPEED(7),   FST_FALL,                       DAMAGE(0), {BASE_WBOX }},
        { 50,  0,       MOVE(-8,0), "dummy-fall", 1, 4, SPEED(7),   FST_FALL,                       DAMAGE(0), {BASE_WBOX }},
        { 51,  0,       MOVE(-8,0), "dummy-fall", 2, 4, SPEED(7),   FST_FALL,                       DAMAGE(0), {BASE_WBOX }},
        { 52,  0,       MOVE(0,0), "dummy-fall", 3, 4, SPEED(0),    FST_FALL,                        DAMAGE(0), {BASE_WBOX }},
        { 53,  0,       MOVE(0,0), "dummy-fall", 3, 4, SPEED(0),    FST_FALL,                        DAMAGE(0), {BASE_WBOX }},
        { 54,  0,       MOVE(0,0), "dummy-fall", 3, 4, SPEED(0),    FST_FALL,                        DAMAGE(0), {BASE_WBOX }},
        { 55,  0,       MOVE(0,0), "dummy-fall", 3, 4, SPEED(0),    FST_FALL,                        DAMAGE(0), {BASE_WBOX }},
        { 56,  0,       MOVE(0,0), "dummy-fall", 3, 4, SPEED(0),    FST_FALL,                        DAMAGE(0), {BASE_WBOX }},
        { 57,  NEXT(0), MOVE(0,0), "dummy-fall", 3, 4, SPEED(0),    FST_FALL,                        DAMAGE(0), {BASE_WBOX }},
        /*********************************************************************************************************************************/
        { 58, 0,        MOVE(8,5),   "dummy-jumpkick", 0, 3, SPEED(8),  FST_JMP,                    DAMAGE(8), {BASE_WBOX, BASE_BBOX}},
        { 59, 0,        MOVE(8,10),  "dummy-jumpkick", 1, 3, SPEED(8),  FST_JMP,                    DAMAGE(8), {BASE_WBOX, BASE_BBOX}},
        { 60, NEXT(0),  MOVE(8,15),  "dummy-jumpkick", 2, 3, SPEED(8),  FST_JMP,                    DAMAGE(8), {BASE_WBOX, BASE_BBOX}},
        /*********************************************************************************************************************************/
        { 61, 0,        MOVE(8,5),   "dummy-jumppunch", 0, 3, SPEED(8),  FST_JMP,                    DAMAGE(6), {BASE_WBOX, BASE_BBOX}},
        { 62, 0,        MOVE(8,10),  "dummy-jumppunch", 1, 3, SPEED(8),  FST_JMP,                    DAMAGE(6), {BASE_WBOX, BASE_BBOX}},
        { 63, NEXT(0),  MOVE(8,15),  "dummy-jumppunch", 2, 3, SPEED(8),  FST_JMP,                    DAMAGE(6), {BASE_WBOX, BASE_BBOX}},

        { 0, 0 },
    },
    { // keyframes
        /* idle */   0,
        /* fwd  */   5,
        /* bwd  */   13,
        /* jump */   21,
        /* punch */  28,
        /* kick */   33,
        /* hit */    38,
        /* special */41,
        /* fall */   49,
        /* jkick */  58,
        /* jpunch */ 61,
    },
    /* pivot */ 45,119,
    /* palette */ "dummy-idle.pal.bin",
    /* special */ { TYPE_HAGOKEN, 64, -40 },
};
