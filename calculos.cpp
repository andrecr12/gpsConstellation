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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include "calculos.h"

/*  Estruturas de dados  */
typedef struct Satellite{
    char name[2];
    double orbitalAngle;
    double initialAngle;
    char orbit;
}Satellite;

typedef struct ClosestSatellite{
    char name[2];
    double x;
    double y;
    double z;
    double distance;
    int index;
}ClosestSatellite;

typedef struct User{
    double x;
    double y;
    double z;
    double latDegree;
    double latMin;
    double latSec;
    double lonDegree;
    double lonMin;
    double lonSec;
    double altitude;
}User;

/*  Variáveis Globais  */
Satellite sat[SATELLITEQTT];
ClosestSatellite closestSat[3];
User user;
User userGps;

double sint[SINCOSQTT];
double cost[SINCOSQTT];
double delta = 360*DEGREE2RADIAN/SINCOSQTT;

/*
 * Função que inicia os valores do usuário GPS
 */
void initUser()
{
    user.x = 0.5;
    user.y = 0.0;
    user.z = 1.0;
    //user.z = 0.867;
    user.latDegree = 0;
    user.latMin = 0;
    user.latSec = 0.0;
    user.lonDegree = 0;
    user.lonMin = 0;
    user.lonSec = 0.0;
    user.altitude = 0.0;

    userGps.x = 0;
    userGps.y = 0;
    userGps.z = 0;
    userGps.latDegree = 0;
    userGps.latMin = 0;
    userGps.latSec = 0.0;
    userGps.lonDegree = 0;
    userGps.lonMin = 0;
    userGps.lonSec = 0.0;
    userGps.altitude = 0.0;
}

/*
 * Função que inicia variáveis dos satélites
 */
void initSatellites()
{
    int i;

    for (i=0; i<SATELLITEQTT; i++){
        switch(i/4){
            case 0:             //órbita A
                sat[i].name[0] = 'A';
                sat[i].name[1] = '0'+(i%4)+1;
                sat[i].orbit = 'A';
                sat[i].initialAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4);
                sat[i].orbitalAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4);
            break;
            case 1:             //órbita B
                sat[i].name[0] = 'B';
                sat[i].name[1] = '0'+(i%4)+1;
                sat[i].orbit = 'B';
                sat[i].initialAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/6);
                sat[i].orbitalAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/6);
            break;
            case 2:             //órbita C
                sat[i].name[0] = 'C';
                sat[i].name[1] = '0'+(i%4)+1;
                sat[i].orbit = 'C';
                sat[i].initialAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/3);
                sat[i].orbitalAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/3);
            break;
            case 3:             //órbita D
                sat[i].name[0] = 'D';
                sat[i].name[1] = '0'+(i%4)+1;
                sat[i].orbit = 'D';
                sat[i].initialAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4);
                sat[i].orbitalAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4);
            break;
            case 4:             //órbita E
                sat[i].name[0] = 'E';
                sat[i].name[1] = '0'+(i%4)+1;
                sat[i].orbit = 'E';
                sat[i].initialAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/6);
                sat[i].orbitalAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/6);
            break;
            case 5:             //órbita F
                sat[i].name[0] = 'F';
                sat[i].name[1] = '0'+(i%4)+1;
                sat[i].orbit = 'F';
                sat[i].initialAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/3);
                sat[i].orbitalAngle = (M_PI/2)*(i%4) + (M_PI/12)*(i/4) - (M_PI/3);
            break;
            }
    }
}

/*
 * Função que calcula os senos e cossenos para a simulação, minimizando o processamento
 * feito em tempo real na simulação
 */
void initSenCos()
{
    int i;

    for(i=0; i<SINCOSQTT; i++)
    {
        sint[i] = sin(i*delta);
        cost[i] = cos(i*delta);
    }
}

/*
 * Função que define a movimentação fixa do usuário GPS
 */
void updateUserPosition(double increment)
{
    if(user.x >= 0.5 && user.y >= 0.5)
    {
        user.x -= increment;
    }
    else if(user.x <= -0.5 && user.y >= 0.5)
    {
        user.y -= increment;
    }
    else if(user.x <= -0.5 && user.y <= -0.5)
    {
        user.x += increment;
    }
    else if(user.x >= 0.5 && user.y <= -0.5)
    {
        user.y += increment;
    }
    else if(user.x >= 0.5 && user.y > -0.5 && user.y < 0.5)
    {
        user.y += increment;
        if(user.y > 0.5)   user.y = 0.5;
    }
    else if(user.y >= 0.5 && user.x > -0.5 && user.x < 0.5)
    {
        user.x -= increment;
        if(user.x < -0.5)   user.x = -0.5;
    }
    else if(user.x <= -0.5 && user.y > -0.5 && user.y < 0.5)
    {
        user.y -= increment;
        if(user.y < -0.5)   user.y = -0.5;
    }
    else if(user.y <= -0.5 && user.x > -0.5 && user.x < 0.5)
    {
        user.x += increment;
        if(user.x > 0.5)   user.x = 0.5;
    }

}

/*
 * Funções que retornam variáveis das estruturas de dados
 */
double getUserX()
{
    return user.x;
}

double getUserY()
{
    return user.y;
}

double getUserZ()
{
    return user.z;
}

double getUserGpsX()
{
    return userGps.x;
}

double getUserGpsY()
{
    return userGps.y;
}

double getUserGpsZ()
{
    return userGps.z;
}

double getUserGpsLonDegree()
{
    return userGps.lonDegree;
}

double getUserGpsLonMin()
{
    return userGps.lonMin;
}

double getUserGpsLonSec()
{
    return userGps.lonSec;
}

double getUserGpsLatDegree()
{
    return userGps.latDegree;
}

double getUserGpsLatMin()
{
    return userGps.latMin;
}

double getUserGpsLatSec()
{
    return userGps.latSec;
}

double getUserGpsAltitude()
{
    return userGps.altitude;
}

/*
 * Função que retorna valores dos senos calculados inicialmente
 */
double getSint(int index)
{
    return sint[index];
}

/*
 * Função que retorna valores dos cossenos calculados inicialmente
 */
double getCost(int index)
{
    return cost[index];
}

/*
 * Função que retorna o intervalo entre 2 valores consecutivos dos senos e cossenos
 */
double getDelta()
{
    return delta;
}

/*
 * Função que atualiza o ângulo orbital para translação dos satélites
 */
void updateOrbitalAngles(double increment)
{
    int i;

    for (i=0; i<SATELLITEQTT; i++)  sat[i].orbitalAngle = sat[i].initialAngle + increment;
}

/*
 * Função que rotaciona um ponto (x,y,z) em relação ao eixo y
 */
void roty(double pos[], double angle)
{
    double x1, y1, z1;
    int index;

    while(angle > 2*M_PI)  angle -= 2*M_PI;

    index = angle/delta;

    x1 =  pos[0]*cost[index] + 0 + pos[2]*sint[index] + 0;
    y1 = 0 + pos[1] + 0 + 0;
    z1 = -pos[0]*sint[index] + 0 + pos[2]*cost[index] + 0;

    pos[0] = x1;
    pos[1] = y1;
    pos[2] = z1;
}

/*
 * Função que rotaciona um ponto (x,y,z) em relação ao eixo z
 */
void rotz(double pos[], double angle)
{
    double x1, y1, z1;
    int index;

    while(angle > 2*M_PI)  angle -= 2*M_PI;

    index = angle/delta;

    x1 = pos[0]*cost[index] - pos[1]*sint[index] + 0;
    y1 = pos[0]*sint[index] + pos[1]*cost[index] + 0;
    z1 = 0 + 0 + pos[2];

    pos[0] = x1;
    pos[1] = y1;
    pos[2] = z1;
}

/*
 * Função que calcula a posição do satélite s no momento da simulação epoch,
 * armazenando em posXYZ[]
 */
void calcSatPosition(double *posXYZ, int s, float epoch)
{
    double angle = sat[s].orbitalAngle;
    int index;

    if (angle < 0)  angle += 2*M_PI;

    while(angle > 2*M_PI)   angle -= 2*M_PI;

    index = angle/delta;

    posXYZ[0] =  (ORBITR/EARTHR)*cost[index];
    posXYZ[1] = 0;
    posXYZ[2] = -(ORBITR/EARTHR)*sint[index];

    rotz(posXYZ, 55*(M_PI/180));
    roty(posXYZ, epoch*(M_PI)/180 + (s/4)*(M_PI)/3);

}

/*
 * Função que calcula a distância do usuário GPS com o satélite de posição sat[]
 */
double calcDistanceUserSat(double sat[])
{
    return sqrt(pow(user.x - sat[0], 2) + pow(user.y - sat[1], 2) + pow(user.z - sat[2], 2));
}

/*
 * Função que calcula a distância do satélite próximo sat com (x,y,z)
 */
double calcDistanceSat(ClosestSatellite sat, double x, double y, double z)
{
    return sqrt(pow(sat.x - x, 2) + pow(sat.y - y, 2) + pow(sat.z - z, 2));
}

/*
 * Função que determina 3 satélites próximos do usuário GPS
 */
void calcClosestSatellites(float epoch, int &flagLines)
{
    int i;
    double auxSat[3], newDist;
    flagLines = 1;

    for(i=0; i<SATELLITEQTT; i++)
    {
        calcSatPosition(auxSat, i, epoch);      //calcula posição do satélite i
        if(i < 3)
        {
            closestSat[i].x = auxSat[0];
            closestSat[i].y = auxSat[1];
            closestSat[i].z = auxSat[2];
            closestSat[i].distance = calcDistanceUserSat(auxSat);
            closestSat[i].index = i;
            strcpy(closestSat[i].name, sat[i].name);
        }
        else
        {
            newDist = calcDistanceUserSat(auxSat);
            if(newDist < closestSat[0].distance)
            {
                closestSat[0].x = auxSat[0];
                closestSat[0].y = auxSat[1];
                closestSat[0].z = auxSat[2];
                closestSat[0].distance = newDist;
                closestSat[0].index = i;
                strcpy(closestSat[0].name, sat[i].name);
            }
            else if(newDist < closestSat[1].distance)
            {
                closestSat[1].x = auxSat[0];
                closestSat[1].y = auxSat[1];
                closestSat[1].z = auxSat[2];
                closestSat[1].distance = newDist;
                closestSat[1].index = i;
                strcpy(closestSat[1].name, sat[i].name);
            }
            else if(newDist < closestSat[2].distance)
            {
                closestSat[2].x = auxSat[0];
                closestSat[2].y = auxSat[1];
                closestSat[2].z = auxSat[2];
                closestSat[2].distance = newDist;
                closestSat[2].index = i;
                strcpy(closestSat[2].name, sat[i].name);
            }
        }
    }
}

/*
 * Função que retorna coordenadas da posição de um satélite próximo
 */
double getValueOfClosestSats(int sat, int coord)
{
    if (coord == 0)
    {
        return closestSat[sat].x;
    }
    else if (coord == 1)
    {
        return closestSat[sat].y;
    }
    else
    {
        return closestSat[sat].z;
    }
}

/*
 * Função que retorna nome do satélite de índice sat
 */
char* getNameOfClosestSats(int sat)
{
    return closestSat[sat].name;
}

/*
 * Função que resolve sistemas de ordem 3 por Eliminação de Gauss (escalonamento)
 */
void gaussElim(double A[][3], double B[], double X[])
{
    double r;

    //elimina a21
    r = A[1][0]/A[0][0];
    A[1][0] -= A[0][0]*r;
    A[1][1] -= A[0][1]*r;
    A[1][2] -= A[0][2]*r;
    B[1] -= B[0]*r;

    //elimina a31
    r = A[2][0]/A[0][0];
    A[2][0] -= A[0][0]*r;
    A[2][1] -= A[0][1]*r;
    A[2][2] -= A[0][2]*r;
    B[2] -= B[0]*r;

    //elimina a32
    r = A[2][1]/A[1][1];
    A[2][1] -= A[1][1]*r;
    A[2][2] -= A[1][2]*r;
    B[2] -= B[1]*r;

    X[2] = B[2]/A[2][2];
    X[1] = (B[1] - X[2]*A[1][2])/A[1][1];
    X[0] = (B[0] - X[1]*A[0][1] - X[2]*A[0][2])/A[0][0];
}

/*
 * Função que converte um valor real value em graus, min e seg
 */
void convertReal2DegMinSec(double value, unsigned short int flagLat)
{
    double aux;

    if (!flagLat)
    {
        aux = modf(value, &userGps.lonDegree)*60;
        if(value < 0)   aux *= -1;
        userGps.lonSec = modf(aux, &userGps.lonMin)*60;
    }
    else
    {
        aux = modf(value, &userGps.latDegree)*60;
        if(value < 0)   aux *= -1;
        userGps.latSec = modf(aux, &userGps.latMin)*60;
    }
}

/*
 * Função que converte de (x,y,z) para WGS84 pelo método de Bowring,
 * retornando a quantidade de iterações necessárias
 */
int convertXyz2Wgs()
{
    int count = 0;
    double p, a, b, e, e2, N;
    double sinU, cosU, tanU0, tanU, fi, tanFi;
    double x, y, z;
    double convRad2Deg = 180/M_PI;      //conversão de radianos para graus

    //definido pelo WGS84
    a = 6378137.0;
    b = 6356752.31;

    x = userGps.z*a;
    y = userGps.x*a;
    z = userGps.y*b;
    e = sqrt(1-(pow(b,2)/pow(a,2)));
    e2 = (a/b)*e;

    //calcula longitude
    if(x >= 0)
    {
        convertReal2DegMinSec(atan(y/x)*convRad2Deg, 0);
    }
    else if(x < 0 && y >= 0)
    {
        convertReal2DegMinSec(180.0 + atan(y/x)*convRad2Deg, 0);
    }
    else if(x < 0 && y < 0)
    {
        convertReal2DegMinSec(-180.0 + atan(y/x)*convRad2Deg, 0);
    }

    p = sqrt(pow(x, 2) + pow(y, 2));
    tanU = (z/p)*(a/b);

    do{         //laço iterativo
        count++;

        tanU0 = tanU;
        cosU = sqrt(1/(1 + pow(tanU0, 2)));
        sinU = sqrt(1 - pow(cosU, 2));
        tanFi = (z + pow(e2, 2)*b*pow(sinU, 3))/(p - pow(e, 2)*a*pow(cosU, 3));
        tanU = (b/a)*tanFi;

    } while(abs(tanU0 - tanU) > 1e-6);

    fi = atan(tanFi);
    convertReal2DegMinSec(fi*convRad2Deg, 1);   //armazena latitude

    N = a/(sqrt(1 - pow(e, 2)*pow(sin(fi), 2)));

    //calcula altitude
    if(abs(fi - 90.0) > 0.001 && abs(fi + 90.0) > 0.001)
    {
        userGps.altitude = (p/cos(fi)) - N;
        userGps.altitude /= 1E3;
    }
    else if(fi > 0.001)
    {
        userGps.altitude = (z/sin(fi)) - N + pow(e, 2)*N;
        userGps.altitude /= 1E3;
    }

    return count;
}

/*
 * Função que calcula a posição do usuário GPS pelo método iterativo proposto
 * em Kaplan, 2006. Armazena valores dos cálculos no arquivo log.txt e retorna
 * a quantidade de iterações necessárias
 */
int calcUserGpsPosition(FILE *fp)
{
    int i, count = 0;
    double r[3];
    double d[3];

    double A[3][3];
    double X[3];
    double B[3];

    double xt, yt, zt;

    xt = 0;
    yt = 0;
    zt = 0;

    do{                 //laço iterativo
        count++;
        fprintf(fp, "\nIteracao: %d", count);

        for(i=0; i<3; i++){
            r[i] = calcDistanceSat(closestSat[i], xt, yt, zt);
            d[i] = closestSat[i].distance;
            B[i] = r[i] - d[i];
        }
        fprintf(fp, "\nr1: %lf\t\tr2: %lf\t\tr3: %lf", r[0], r[1], r[2]);
        fprintf(fp, "\n∆d1: %lf\t\t∆d2: %lf\t\t∆d3: %lf\t\t", B[0], B[1], B[2]);

        for(i=0; i<3; i++){
            A[i][0] = (closestSat[i].x - xt)/r[i];
            A[i][1] = (closestSat[i].y - yt)/r[i];
            A[i][2] = (closestSat[i].z - zt)/r[i];
            fprintf(fp, "\nH[%d][1]: %lf\tH[%d][2]: %lf\tH[%d][3]: %lf", i+1, A[i][0], i+1, A[i][1], i+1, A[i][2]);
        }

        //calculo do sistema de ordem 3
        gaussElim(A, B, X);

        userGps.x = xt + X[0];
        userGps.y = yt + X[1];
        userGps.z = zt + X[2];
        fprintf(fp, "\n∆x: %lf\t\t∆y: %lf\t\t∆z: %lf\n", X[0], X[1], X[2]);

        xt = userGps.x;
        yt = userGps.y;
        zt = userGps.z;

    } while((X[0] > 0.00001) || (X[1] > 0.00001) || (X[2] > 0.00001) || (X[0] < -0.00001) || (X[1] < -0.00001) || (X[2] < -0.00001));

    fprintf(fp, "Posicao calculada do usuario: %lf, %lf, %lf\n\n", xt, yt, zt);

    return count;
}
