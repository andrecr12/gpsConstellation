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


/*  Declaraçoes  */
#define EARTHR 6370.0
#define ORBITR 20200.0
#define SATELLITEQTT 24
#define SINCOSQTT 10000
#define DEGREE2RADIAN M_PI/180


/*  Biblioteca do código de calculos.cpp  */
void initUser();

void initSatellites();

void initSenCos();

void updateUserPosition(double increment);

double getUserX();

double getUserY();

double getUserZ();

double getUserGpsX();

double getUserGpsY();

double getUserGpsZ();

double getUserGpsLonDegree();

double getUserGpsLonMin();

double getUserGpsLonSec();

double getUserGpsLatDegree();

double getUserGpsLatMin();

double getUserGpsLatSec();

double getUserGpsAltitude();

double getSint(int index);

double getCost(int index);

double getDelta();

void updateOrbitalAngles(double increment);

void calcClosestSatellites(float epoch, int &flagLines);

double getValueOfClosestSats(int sat, int coord);

char* getNameOfClosestSats(int sat);

int convertXyz2Wgs();

int calcUserGpsPosition(FILE *fp);
