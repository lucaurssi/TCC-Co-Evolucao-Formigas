#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif


using namespace std;

#define windowWidth 900
#define windowHeight 900


void interface(){
    return;
}


int main(int argc, char** argv){
  
    //----- Create Window -----//
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("TCC-Co-Evolucao-Formigas");
    glClearColor(1.0, 1.0, 1.0, 1.0);// clear screen

    glutDisplayFunc(interface);// defining drawing function

    //glutTimerFunc(0, timer, 0); // clock, processing function here
	//glutMouseFunc(button_click);// mouse click handler

    glutMainLoop();
    return 0;
}
