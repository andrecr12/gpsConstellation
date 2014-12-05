/*
 ****************************************************************************
 *                                                                          *
 * GPS Constellation                                                        *
 * Simulação do TCC: Estudo do Sistema de Posicionamento Global (GPS)       *
 * Autor: André Carrasco Rodrigues      NUSP: 5971243                       *
 * Engenharia da Computação - EESC/USP                                      *
 * Ano: 2011                                                                *
 *                                                                          *
 ****************************************************************************
 */

#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include "calculos.h"

/*  Declarações */
#define CIRCLEPOINTS 100

/*  Variáveis Globais  */
GLuint window, info, space;
GLuint spaceWidth = 720, spaceHeight = 720;
GLuint infoWidth = 480, infoHeight = 720;
GLuint sub_width = 720, sub_height = 720;
GLuint GAP = 10;
GLvoid *font_style;

float epoch = 0.0;
float speed = 1.0;
int flagLines = 0, flagStop = 0;
int flagEquaVision = 1, flagNorthPoleVision = 0, flagSouthPoleVision = 0;
int count1 = 0, count2 = 0;
long long int last_time = 0;
double userIncr = 0.0005;

FILE *fp;

/*
 * Função que escreve uma string em uma posição (x,y) em pixels
 */
void drawstr(GLuint x, GLuint y, char* format, ...)
{
    va_list args;
    char buffer[255], *s;

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    glRasterPos2i(x, y);
    for (s = buffer; *s; s++)
        glutBitmapCharacter(font_style, *s);
}

/*
 * Função que desenha a Terra e seus paralelos e meridianos com centro em (0,0,0)
 */
void drawEarth()
{
    glPushMatrix();
        glColor3f (0.2, 0.2, 1.0);
        glRotatef(90.0, 1.0, 0.0, 0.0);
        glutSolidSphere(1.0, 16, 32);
        glColor3f (0.0, 0.0, 0.0);
        glutWireSphere(1.02, 18, 30);
    glPopMatrix();
}

/*
 * Função que desenha o usuário GPS na sua posição atual
 */
void drawUser()
{
    glPushMatrix();
        glColor3f (1.0, 1.0, 1.0);
        glTranslatef((GLfloat)getUserX(), (GLfloat)getUserY(), (GLfloat)getUserZ());
        glutSolidSphere(0.05, 8, 16);
    glPopMatrix();
}

/*
 * Função que desenha um satélite
 */
void drawSatellite(int orbit, double angle)
{
    double angleAux = angle + orbit*(M_PI/12);
    int index = angleAux/getDelta();

    glPushMatrix();
        glTranslatef((ORBITR/EARTHR)*getCost(index), 0, (ORBITR/EARTHR)*getSint(index));
        glRotatef(-55.0, 0.0, 0.0, 1.0);
        glColor3f(0.5, 0.5, 0.5);
        glutSolidCube(0.125);
        glColor3f(0.0, 0.0, 0.0);
        glutWireCube(0.126);
        glColor3f(0.7, 0.7, 0.7);
        glBegin(GL_LINES);                  //eixo central
            glVertex3f( 0.18, 0.0, 0.0);
            glVertex3f(-0.18, 0.0, 0.0);
        glEnd();
        glBegin(GL_QUADS);                  //painel solar 1
            glVertex3f(-0.180, 0.1,-0.1);
            glVertex3f(-0.180,-0.1, 0.1);
            glVertex3f(-0.065,-0.1, 0.1);
            glVertex3f(-0.065, 0.1,-0.1);
        glEnd();
        glBegin(GL_QUADS);                  //painel solar 2
            glVertex3f( 0.065, 0.1,-0.1);
            glVertex3f( 0.065,-0.1, 0.1);
            glVertex3f( 0.180,-0.1, 0.1);
            glVertex3f( 0.180, 0.1,-0.1);
        glEnd();
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);
            glVertex3f(-0.180, 0.1,-0.1);
            glVertex3f(-0.065,-0.1, 0.1);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f(-0.180,-0.1, 0.1);
            glVertex3f(-0.065, 0.1,-0.1);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f( 0.065, 0.1,-0.1);
            glVertex3f( 0.180,-0.1, 0.1);
        glEnd();
        glBegin(GL_LINES);
            glVertex3f( 0.065,-0.1, 0.1);
            glVertex3f( 0.180, 0.1,-0.1);
        glEnd();
    glPopMatrix();

}

/*
 * Função que desenha as linhas dos 3 satélites mais próximos
 */
void drawClosestSatLines()
{
    int i;

    for(i=0; i<3; i++)
    {
        glPushMatrix();
            glBegin(GL_LINES);
                if(i==0)    glColor3f(1.0, 0.412, 0.706);           //linha laranja
                else if(i==1)    glColor3f(0.33, 0.745, 0.33);      //linha verde
                else if(i==2)    glColor3f(1.0, 0.0, 0.0);          //linha vermelha
                glVertex3f((GLfloat)getUserX(), (GLfloat)getUserY(), (GLfloat)getUserZ());
                glVertex3f((GLfloat)getValueOfClosestSats(i, 0), (GLfloat)getValueOfClosestSats(i, 1), (GLfloat)getValueOfClosestSats(i, 2));
            glEnd();
        glPopMatrix();
    }
}

/*
 * Função que define parâmetros OpenGL para janela principal
 */
void main_display()
{
    glClearColor(0.6, 0.6, 0.6, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
}

/*
 * Função que define alteração do tamanho da janela principal
 */
void main_reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    sub_width = w - 3*GAP - infoWidth;
    sub_height = h - 2*GAP;

    glutSetWindow(space);           //janela space com tamanho variado
    glutPositionWindow(GAP, GAP);
    glutReshapeWindow(sub_width, sub_height);
    glutSetWindow(info);            //janela info com largura fixa
    glutPositionWindow(GAP+sub_width+GAP, GAP);
    glutReshapeWindow(infoWidth, sub_height);
}

/*
 * Função que desenha a janela space
 */
void space_display(void)
{
    int i, orbit, index;
    double angle;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawEarth();

    drawUser();

    for(orbit=0; orbit < 6; orbit++)    //desenha orbitas
    {
        glPushMatrix();
            glRotatef ((GLfloat) epoch, 0.0, 1.0, 0.0);     //rotação terrestre
            glRotatef(60.0*orbit, 0.0, 1.0, 0.0);
            glRotatef(55.0, 0.0, 0.0, 1.0);
            glBegin(GL_LINE_LOOP);
                glColor3f(1.0, 1.0, 0.0);

                for(i=0; i<CIRCLEPOINTS; i++){      //desenha orbita amarela
                    angle = i*2*M_PI/CIRCLEPOINTS;
                    index = angle/getDelta();
                    glVertex3f((ORBITR/EARTHR)*getCost(index), 0, (ORBITR/EARTHR)*getSint(index));
                }
            glEnd();

            glRotatef((GLfloat) (2*epoch), 0.0, 1.0, 0.0);      //translação orbital
            for(i=0; i<CIRCLEPOINTS; i++)
            {
                if(i%(CIRCLEPOINTS/4) == 0)     //desenha 4 satélites por órbita
                {
                    angle = 2*M_PI*i/CIRCLEPOINTS;
                    drawSatellite(orbit, angle);
                }
            }
        glPopMatrix();
    }

    if(flagLines)       //evita desenhar linhas antes de calcular os satélites mais próximos
    {
        drawClosestSatLines();
    }

    glutSwapBuffers();
}

/*
 * Função que define o tipo de visão no OpenGL da janela space
 */
void space_reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.1, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(flagEquaVision)
        gluLookAt (0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    else if(flagNorthPoleVision)
        gluLookAt (0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
    else if(flagSouthPoleVision)
        gluLookAt (0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    glEnable (GL_DEPTH_TEST);
}

/*
 * Função que define o tipo de visão no OpenGL da janela info
 */
void info_reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

/*
 * Função que desenha a janela info
 */
void info_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    font_style = GLUT_BITMAP_HELVETICA_12;

    //escreve informações na tela
    glColor3d(1, 1, 1);
    drawstr(20, 20, "Satelites:");

    //posições dos satélites mais próximos
    glColor3f(1.0, 0.412, 0.706);
    drawstr(20, 40, "%s - ", getNameOfClosestSats(0));
    drawstr(60, 40, "X:");
    drawstr(80, 40, "%0.4f", getValueOfClosestSats(0, 0));
    drawstr(200, 40, "Y:");
    drawstr(220, 40, "%0.4f", getValueOfClosestSats(0, 1));
    drawstr(340, 40, "Z:");
    drawstr(360, 40, "%0.4f", getValueOfClosestSats(0, 2));

    glColor3f(0.33, 0.745, 0.33);
    drawstr(20, 60, "%s - ", getNameOfClosestSats(1));
    drawstr(60, 60, "X:");
    drawstr(80, 60, "%0.4f", getValueOfClosestSats(1, 0));
    drawstr(200, 60, "Y:");
    drawstr(220, 60, "%0.4f", getValueOfClosestSats(1, 1));
    drawstr(340, 60, "Z:");
    drawstr(360, 60, "%0.4f", getValueOfClosestSats(1, 2));

    glColor3f(1.0, 0.0, 0.0);
    drawstr(20, 80, "%s - ", getNameOfClosestSats(2));
    drawstr(60, 80, "X:");
    drawstr(80, 80, "%0.4f", getValueOfClosestSats(2, 0));
    drawstr(200, 80, "Y:");
    drawstr(220, 80, "%0.4f", getValueOfClosestSats(2, 1));
    drawstr(340, 80, "Z:");
    drawstr(360, 80, "%0.4f", getValueOfClosestSats(2, 2));

    //posição real do usuário definido em calculos.cpp
    glColor3d(1, 1, 1);
    drawstr(20, 120, "Posicao real do usuario");
    drawstr(60, 140, "X:");
    drawstr(80, 140, "%0.4f", getUserX());
    drawstr(200, 140, "Y:");
    drawstr(220, 140, "%0.4f", getUserY());
    drawstr(340, 140, "Z:");
    drawstr(360, 140, "%0.4f", getUserZ());

    //posição calculada do usuário pelo GPS, tanto (x,y,z) quanto (lat,lon,alt)
    glColor3d(1, 1, 1);
    drawstr(20, 180, "Posicao calculada do usuario");
    drawstr(60, 200, "X:");
    drawstr(80, 200, "%0.4f", getUserGpsX());
    drawstr(200, 200, "Y:");
    drawstr(220, 200, "%0.4f", getUserGpsY());
    drawstr(340, 200, "Z:");
    drawstr(360, 200, "%0.4f", getUserGpsZ());
    drawstr(20, 220, "Lat:");
    drawstr(45, 220, "%3.f graus %3.f'%0.1f\"", getUserGpsLatDegree(), getUserGpsLatMin(), getUserGpsLatSec());
    drawstr(175, 220, "Long:");
    drawstr(210, 220, "%3.f graus %3.f'%0.1f\"", getUserGpsLonDegree(), getUserGpsLonMin(), getUserGpsLonSec());
    drawstr(340, 220, "Alt (km):");
    drawstr(390, 220, "%0.4f", getUserGpsAltitude());

    //quantidade de iterações dos métodos matemáticos
    drawstr(20, 240, "No de iteracoes (XYZ): ");
    drawstr(160, 240, "%d", count1);
    drawstr(190, 240, "No de iteracoes (XYZ p/ WGS84): ");
    drawstr(390, 240, "%d", count2);

    //interface com o usuario
    glColor3d(1, 1, 1);
    drawstr(20, 540, "Opcoes:");
    if(flagEquaVision)
    {
        drawstr(40, 560, "- N: Visao do Polo Norte");
        drawstr(40, 580, "- S: Visao do Polo Sul");
    }
    else if(flagNorthPoleVision)
    {
        drawstr(40, 560, "- E: Visao do Equador");
        drawstr(40, 580, "- S: Visao do Polo Sul");
    }
    else if(flagSouthPoleVision)
    {
        drawstr(40, 560, "- N: Visao do Polo Norte");
        drawstr(40, 580, "- E: Visao do Equador");
    }
    drawstr(40, 600, "- P: pausa/reinicia simulacao");
    drawstr(40, 620, "- (seta p/ cima): aumenta velocidade do usuario");
    drawstr(40, 640, "- (seta p/ baixo): diminui velocidade do usuario");
    drawstr(40, 660, "- (seta p/ esquerda): aumenta velocidade orbital");
    drawstr(40, 680, "- (seta p/ direita): diminui velocidade orbital");
    drawstr(40, 700, "- Esc: fecha o programa");

    glutSwapBuffers();
}

/*
 * Função que define para redesenhar todas as janelas
 */
void redisplay_all(void)
{
    glutSetWindow(space);
    space_reshape(sub_width, sub_height);
    glutPostRedisplay();
    glutSetWindow(info);
    info_reshape(infoWidth, sub_height);
    glutPostRedisplay();
}

/*
 * Função que trata eventos do teclado
 */
void keyboard (unsigned char key, int x, int y)
{

    switch (key) {
        case 'e':
        case 'E':
            flagEquaVision = 1;
            flagNorthPoleVision = 0;
            flagSouthPoleVision = 0;
            break;
        case 'n':
        case 'N':
            flagEquaVision = 0;
            flagNorthPoleVision = 1;
            flagSouthPoleVision = 0;
            break;
        case 's':
        case 'S':
            flagEquaVision = 0;
            flagNorthPoleVision = 0;
            flagSouthPoleVision = 1;
            break;
        case 'p':
        case 'P':
            if(flagStop)
            {
                flagStop = 0;
                fprintf(fp, "\nReiniciou simulacao.\nTempo de execucao: %lf seg\n", glutGet(GLUT_ELAPSED_TIME)/1E3);
            }
            else
            {
                flagStop = 1;
                fprintf(fp, "\nPausou simulacao.\nTempo de execucao: %lf seg\n", glutGet(GLUT_ELAPSED_TIME)/1E3);
            }
            break;
        case 27:
            fprintf(fp, "\nFinalizando simulacao.\nTempo de execucao: %lf seg", glutGet(GLUT_ELAPSED_TIME)/1E3);
            if(!fclose(fp))
                exit(0);
            break;
      default:
         break;
   }
}

/*
 * Função que trata eventos do teclado para teclas especiais (setas)
 */
void specialKeys(int key, int x, int y)
{

    switch (key) {
        case GLUT_KEY_RIGHT:
            speed += 0.1;
            fprintf(fp, "\nAumentou velocidade das orbitas.\nTempo de execucao: %lf seg\n", glutGet(GLUT_ELAPSED_TIME)/1E3);
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:
            if((speed - 0.1) > 0)
            {
                speed -= 0.1;
                fprintf(fp, "\nDiminuiu velocidade das orbitas.\nTempo de execucao: %lf seg\n", glutGet(GLUT_ELAPSED_TIME)/1E3);
            }
            glutPostRedisplay();
            break;
        case GLUT_KEY_UP:
            userIncr += 0.00025;
            fprintf(fp, "\nAumentou velocidade do usuario.\nTempo de execucao: %lf seg\n", glutGet(GLUT_ELAPSED_TIME)/1E3);
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:
            if((userIncr - 0.00025) > 0)
            {
                userIncr -= 0.00025;
                fprintf(fp, "\nDiminuiu velocidade do usuario.\nTempo de execucao: %lf seg\n", glutGet(GLUT_ELAPSED_TIME)/1E3);
            }
            glutPostRedisplay();
            break;
        default:
            break;
   }
}

/*
 * Função que chama os cálculos a cada 100 ms
 */
void idle(void)
{
    if((glutGet(GLUT_ELAPSED_TIME) - last_time) > 100)
    {
        last_time = (glutGet(GLUT_ELAPSED_TIME));

        if(!flagStop)   //caso não tenha pausado a simulação
        {
            updateUserPosition(userIncr);
            updateOrbitalAngles(2*epoch*M_PI/180);
            calcClosestSatellites(epoch, flagLines);
            count1 = calcUserGpsPosition(fp);
            count2 = convertXyz2Wgs();
        }
    }
    redisplay_all();
}

/*
 * Função que define o timer para incrementar variáveis responsáveis pela
 * rotação terrestre e translação dos satélites a cada 50 ms
 */
void timer(int value)
{
    if(!flagStop)
        epoch += (1.0/20.0)*speed;

    redisplay_all();
    glutTimerFunc(50, timer, 1);
}

/*
 * Função main
 */
int main(int argc, char** argv)
{
    printf("Iniciando simulacao...");
    initSenCos();       //calcula senos e cossenos pra simulação
    fp = fopen("log.txt", "w");
    fprintf(fp, "Iniciando simulacao...\n");
    printf("\nPronto!");

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(GAP+spaceWidth+GAP+infoWidth+GAP, GAP+spaceHeight+GAP);
    glutInitWindowPosition(50, 50);
    glutInit(&argc, argv);

    window = glutCreateWindow("GPS Constellation");
    glutDisplayFunc(main_display);
    glutReshapeFunc(main_reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    space = glutCreateSubWindow(window, GAP, GAP, spaceWidth, spaceHeight);
    glutDisplayFunc(space_display);
    glutReshapeFunc(space_reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    info = glutCreateSubWindow(window, GAP+spaceWidth+GAP, GAP, infoWidth, infoHeight);
    glutDisplayFunc(info_display);
    glutReshapeFunc(info_reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    //inicia variáveis da simulação
    initUser();
    initSatellites();

    //define início do timer
    glutTimerFunc(50, timer, 1);

    redisplay_all();
    glutMainLoop();
    return 0;
}
