#ifndef  _H_MACHINE_H_
#define  _H_MACHINE_H_

#define MOTION_NUM  10

char *rst = "$DGS:0!";
char *close = "$WVL_CLOSE!";
char *capture = "$DGT:1-2,1!";
char *raise = "$DGS:3!";
/*¶¯×÷×é*/
char *motion[MOTION_NUM] = {"$DGT:16-17,1!","$DGT:14-15,1!","$DGT:12-13,1!","$DGT:10-11,1!",
                            "$DGT:8-9,1!","$DGT:18-19,1!","$DGT:20-21,1!","$DGT:22-23,1!"
                            ,"$DGT:24-25,1!","$DGT:26-27,1!"};

#define MOTION1 1
#define MOTION2 2
#define MOTION3 3

#endif
