#ifndef IMAGES_H
#define IMAGES_H

#define ForwardFacing 0
#define RightFacing 1
#define BackwardFacing 2
#define LeftFacing 3

//extern const unsigned short bulletImage[];
//extern const unsigned short playerForwardImage[];
//extern const unsigned short playerLeftImage[];
//extern const unsigned short playerRightImage[];
//extern const unsigned short playerReverseImage[];
//extern const unsigned short basicZombieRightImage[];
//extern const unsigned short basicZombieLeftImage[];
//extern const unsigned short basicZombieForwardImage[];
//extern const unsigned short basicZombieBackwardImage[];
//extern const unsigned short bigZombieRightImage[];
//extern const unsigned short bigZombieLeftImage[];
//extern const unsigned short bigZombieForwardImage[];
//extern const unsigned short bigZombieBackwardImage[];
extern const unsigned short startscreen[];
//extern const unsigned short grass[];
//extern const unsigned short grassbackground[];
//extern const unsigned short test[];
extern const unsigned short loser[];

struct spriteImage
{
	const unsigned short *image;
	const int width;
	const int height;
};
typedef struct spriteImage spriteImage_t;

extern const spriteImage_t playerImage[];
extern const spriteImage_t BulletImage[];
extern const spriteImage_t basicZombieImage[];
extern const spriteImage_t bigZombieImage[];
extern const spriteImage_t backgroundOptions[];

extern const spriteImage_t ProfessorImages[];


#endif