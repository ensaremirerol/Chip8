#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>

#include "Chip8.h"


int WIDITH = 640, HEIGHT = 320;
int pixelSize = 10;
time_t currentTime = time(NULL);

const char *ROM_FILE_PATH = "./ROMS";

Chip8 chip8 = Chip8(2.0f);

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	for(int y = 0; y<32; y++){
		for(int x = 0; x<64; x++){
			if(chip8.display[x + y * 64] == 1){
				glColor3f(1,1,1);			
			}
			else{
				glColor3f(0,0,0); 
			}
			
			glBegin(GL_POLYGON);
			glVertex2f((x * pixelSize), HEIGHT - (y * pixelSize));
			glVertex2f(((x+1) * pixelSize), HEIGHT - (y * pixelSize));
			glVertex2f(((x+1) * pixelSize), HEIGHT - ((y+1) * pixelSize));
			glVertex2f((x * pixelSize), HEIGHT - ((y+1) * pixelSize));
			glEnd();
		}
	}
	glFlush();
 	glutSwapBuffers();	
 	
}

void keypadDown(unsigned char key, int x, int y){
	if(key == '1') chip8.keypad[1] = 1;
	if(key == '2') chip8.keypad[2] = 1;
	if(key == '3') chip8.keypad[3] = 1;
	if(key == 'q') chip8.keypad[4] = 1;
	if(key == 'w') chip8.keypad[5] = 1;
	if(key == 'e') chip8.keypad[6] = 1;
	if(key == 'a') chip8.keypad[7] = 1;
	if(key == 's') chip8.keypad[8] = 1;
	if(key == 'd') chip8.keypad[9] = 1;
	if(key == 'z') chip8.keypad[10] = 1;
	if(key == 'c') chip8.keypad[11] = 1;
	if(key == 'v') chip8.keypad[12] = 1;
	if(key == 'f') chip8.keypad[13] = 1;
	if(key == 'r') chip8.keypad[14] = 1;
	if(key == '4') chip8.keypad[15] = 1;
	if(key == 'x') chip8.keypad[0] = 1;
}

void keypadUp(unsigned char key, int x, int y){
	if(key == '1') chip8.keypad[1] = 0;
	if(key == '2') chip8.keypad[2] = 0;
	if(key == '3') chip8.keypad[3] = 0;
	if(key == 'q') chip8.keypad[4] = 0;
	if(key == 'w') chip8.keypad[5] = 0;
	if(key == 'e') chip8.keypad[6] = 0;
	if(key == 'a') chip8.keypad[7] = 0;
	if(key == 's') chip8.keypad[8] = 0;
	if(key == 'd') chip8.keypad[9] = 0;
	if(key == 'z') chip8.keypad[10] = 0;
	if(key == 'c') chip8.keypad[11] = 0;
	if(key == 'v') chip8.keypad[12] = 0;
	if(key == 'f') chip8.keypad[13] = 0;
	if(key == 'r') chip8.keypad[14] = 0;
	if(key == '4') chip8.keypad[15] = 0;
	if(key == 'x') chip8.keypad[0] = 0;
}

void idle(){
	chip8.emulate();
	if(chip8.drawFlag){
		glutPostRedisplay();
		chip8.drawFlag = false;
	} 
}


int main(int argc, char** argv)
{
	if (argc == 1) {
		std::cout << "Enter the path of rom" << std::endl;
		return 1;
	}
	if (!chip8.load(argv[1])) {
		return 1;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIDITH, HEIGHT);
	glutCreateWindow("Chip-8 Emu");
	glutDisplayFunc(display);
	glutKeyboardFunc(keypadDown);
	glutKeyboardUpFunc(keypadUp);
	glutIdleFunc(idle);
	gluOrtho2D(0,WIDITH,0,HEIGHT);
	glClearColor(0,0,0,0);
	glutMainLoop();
	return 0;
}
