#include "graphics.h"
#include "winbgim.h"
#include <windows.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <fstream>

using namespace std;

#define l 1600
#define h 1000
#define nrLin 10
#define nrCol 16
#define margine_stanga 200
#define margine_sus 100
#define culoare_linii RGB(58, 58, 58)
#define culoare_fundal RGB(17, 17, 17)
#define MAX1 20
#define MAX2 100
#define nrTemplates 10

bool inMeniu = true;
bool deselect;
bool drawing = false;
bool paginaVeche;
bool pinAles = false;
int nrComponente = 0;
int nrLegaturi = 0;

struct coordonate {
    float x = 0, y = 0;
};

struct linii {
    coordonate delta1, delta2;
};

struct eclipse {
    coordonate delta;
    float startg, endg;
    float rvertical, rhorizontal;
    float raza;
};

struct piesa {
    float orientare;
    float zoom;
    int id;
    int numarpini;
    bool apasaPin[MAX1] = { false };
    coordonate centru;
    coordonate pin[MAX1] = { (0,0) };
    int numarlinii;
    linii forma[MAX2];
    int numarcercuri;
    eclipse cercuri[MAX2];
}componente[200], templates[10];

struct legatura {
    int pin1;
    int pin2;
    piesa* piesa1;
    int pozitie1;
    piesa* piesa2;
    int pozitie2;
}Conexiuni[1000];

void Legatura(float x1, float y1, float x2, float y2, float culoare)
{
    setcolor(culoare);
    line(x1, y1, (x1 + x2) / 2, y1);
    line((x1 + x2) / 2, y1, (x1 + x2) / 2, y2);
    line((x1 + x2) / 2, y2, x2, y2);
}

void DeseneazaComponenta(piesa componenta) {
    setcolor(WHITE);
    coordonate centru = componenta.centru;
    for (int i = 0; i < componenta.numarlinii; i++) {
        float x1 = componenta.forma[i].delta1.x;
        float y1 = componenta.forma[i].delta1.y;
        float x2 = componenta.forma[i].delta2.x;
        float y2 = componenta.forma[i].delta2.y;
        line(centru.x + x1, centru.y + y1, centru.x + x2, centru.y + y2);
    }
    for (int i = 0; i < componenta.numarcercuri; i++) {
        /*float x = componenta.cercuri[i].delta.x;
        float y = componenta.cercuri[i].delta.y;
        float grads = componenta.cercuri[i].startg;
        float grade = componenta.cercuri[i].endg;
        float height = componenta.cercuri[i].rvertical;
        float length = componenta.cercuri[i].rhorizontal;
        ellipse(x, y, grads, grade, height, length);
        */
        float x1 = componenta.cercuri[i].delta.x;
        float y1 = componenta.cercuri[i].delta.y;
        float s = componenta.cercuri[i].startg;
        float e = componenta.cercuri[i].endg;
        float r = componenta.cercuri[i].raza;
        ellipse(centru.x+x1, centru.y+y1,s,e,r,r);
    }
}

void DeseneazaPin(piesa componenta)
{
    for (int j = 0; j < componenta.numarpini; j++)
    {
        float px = componenta.pin[j].x + componenta.centru.x;
        float py = componenta.pin[j].y + componenta.centru.y;
        rectangle(px - 5, py - 5, px + 5, py + 5);
        setfillstyle(SOLID_FILL, WHITE);
        floodfill(px+1, py+1, WHITE);
    }
}

void DeseneazaLegatura()
{
    for (int i = 0; i < nrLegaturi; i++)
    {
        setcolor(WHITE);
        float coordonate1_x = Conexiuni[i].piesa1->pin[Conexiuni[i].pin1].x + Conexiuni[i].piesa1->centru.x;
        float coordonate1_y = Conexiuni[i].piesa1->pin[Conexiuni[i].pin1].y + Conexiuni[i].piesa1->centru.y;
        float coordonate2_x = Conexiuni[i].piesa2->pin[Conexiuni[i].pin2].x + Conexiuni[i].piesa2->centru.x;
        float coordonate2_y = Conexiuni[i].piesa2->pin[Conexiuni[i].pin2].y + Conexiuni[i].piesa2->centru.y;
        Legatura(coordonate1_x, coordonate1_y, coordonate2_x, coordonate2_y, WHITE);
    }
}

int ApasaButon(float x, float y)
{
    auto latura = l * 0.05 + 10;
    for (float i = 0; i <= 9; i++)
    {
        if (x >= 50 && x <= 150 && y >= (50 + latura * i) && y <= (50 + latura * (i + 1)))
            return i;
    }
    return -1;
}

void ApasaPin(float x, float y) {
    for (int i = 0; i < nrComponente; i++) {
        for (int j = 0; j < componente[i].numarpini; j++) {
            float px = componente[i].pin[j].x + componente[i].centru.x;
            float py = componente[i].pin[j].y + componente[i].centru.y;        
            if (x >= px - 5 && y >= py - 5 && x <= px + 5 && y <= py + 5 && componente[i].apasaPin[j] == false) {
                
                if (drawing == false) {
                    setcolor(LIGHTBLUE);
                    outtextxy(l / 2 - 25, 50, "ACUM DESENEZI LEGATURI");
                    setcolor(WHITE);
                    Conexiuni[nrLegaturi].piesa1 = &componente[i];
                    Conexiuni[nrLegaturi].pin1 = j;
                    Conexiuni[nrLegaturi].pozitie1 = i;
                    drawing = true;
                    componente[i].apasaPin[j] = true;
                    float a, b;
                    a = mousex();
                    b = mousey();
                    while (!ismouseclick(WM_LBUTTONDOWN)) {
                        //vezi cum faci sa desenezi constant, redesenezi tot
                    }
                    clearmouseclick(WM_LBUTTONDOWN);
                    a = mousex();
                    b = mousey();
                    if (ApasaButon(a, b) != -1)
                    {
                        drawing = false;
                        setcolor(culoare_fundal);
                        outtextxy(l / 2 - 25, 50, "ACUM DESENEZI LEGATURI");
                        componente[i].apasaPin[j] = false;
                        return;
                    }
                    ApasaPin(a, b);
                    
                }
                else if(Conexiuni[nrLegaturi].piesa1 != &componente[i])
                {
                    Conexiuni[nrLegaturi].piesa2 = &componente[i];
                    Conexiuni[nrLegaturi].pin2 = j;
                    Conexiuni[nrLegaturi].pozitie2=i;
                    componente[i].apasaPin[j] = true;
                    setcolor(culoare_fundal);
                    outtextxy(l / 2 - 25, 50, "ACUM DESENEZI LEGATURI");
                    setcolor(WHITE);
                    nrLegaturi++;
                    DeseneazaLegatura();
                    drawing = false;
                }
                else if (Conexiuni[nrLegaturi].piesa1 == &componente[i])
                {
                    setcolor(culoare_fundal);
                    outtextxy(l / 2 - 25, 50, "ACUM DESENEZI LEGATURI");
                    setcolor(WHITE);
                    componente[i].apasaPin[j] = false;
                    Conexiuni[nrLegaturi].piesa1->apasaPin[Conexiuni[nrLegaturi].pin1] = false;
                    drawing = false;
                }
                i = nrComponente + 1;
                j = componente[i].numarpini + 1;
            }
        }
    }
}

void CoordonateCursorClick(int& x, int& y)
{
    while (!ismouseclick(WM_LBUTTONDOWN));
    getmouseclick(WM_LBUTTONDOWN, x, y);
    clearmouseclick(WM_LBUTTONDOWN);
}

void PuneComponenta(int k)
{
    int x, y;
    auto latura = l * 0.05;
    while (1 && k != 0 && !inMeniu && !deselect)
    {
        CoordonateCursorClick(x, y);
        if (x > margine_stanga && x < l && y > margine_sus && y < h)
        {       
            componente[nrComponente] = templates[k];
            componente[nrComponente].centru.x = x;
            componente[nrComponente].centru.y = y;
            componente[nrComponente].zoom = 1;
            DeseneazaComponenta(componente[nrComponente]);
            DeseneazaPin(componente[nrComponente]);
            nrComponente++;     
        }
        int k = ApasaButon(x, y);
        if (k == 0)
        {
            cleardevice();
            nrLegaturi = 0;
            inMeniu = true;
            deselect = true;
            break;
        }
        else if (k == nrTemplates - 1)
        {
            deselect = true; 
            setcolor(culoare_fundal);
            outtextxy(l / 2 - 50, 50, "ACUM PUI PIESE");
            setcolor(WHITE);
        }
        else if (k != -1)
        {
            setcolor(LIGHTBLUE);
            outtextxy(l / 2 - 50, 50, "ACUM PUI PIESE");
            setcolor(WHITE);
            PuneComponenta(k);
        }
    }
}

void SaveFile(char nume[50])
{
    ofstream fout(nume);
    fout << nrComponente << endl;
    for (int i = 0; i < nrComponente; i++)
    {
        fout << componente[i].id << " " << componente[i].centru.x << " " << componente[i].centru.y << endl;
    }
    fout << nrLegaturi<<endl;
    for (int i = 0; i < nrLegaturi; i++)
    {
        fout << Conexiuni[i].pozitie1 << " " << Conexiuni[i].pin1<<" "<<Conexiuni[i].pozitie2 << " "<<Conexiuni[i].pin2<<endl;
    }
    fout.close();
}

void LoadFile(char nume[50])
{
    ifstream fin(nume);
    fin >> nrComponente;
    for (int i = 0; i < nrComponente; i++)
    {
        fin >> componente[i].id >> componente[i].centru.x >> componente[i].centru.y;
        DeseneazaComponenta(componente[i]);
        DeseneazaPin(componente[i]);
    }
    fin>> nrLegaturi;
    for (int i = 0; i < nrLegaturi; i++)
    {
        fin >> Conexiuni[i].pozitie1 >> Conexiuni[i].pin1 >> Conexiuni[i].pozitie2 >> Conexiuni[i].pin2;
        Conexiuni[i].piesa1 = &componente[Conexiuni[i].pozitie1];
        Conexiuni[i].piesa2 = &componente[Conexiuni[i].pozitie2];
    }
    fin.close();
}

void LoadFileComponenta(char nume[50],int i)
{
    ifstream fin(nume);
    fin >> templates[i].numarpini;
    for (int j = 0; j < templates[i].numarpini; j++)
        fin >> templates[i].pin[j].x >>templates[i].pin[j].y;
    fin >> templates[i].numarlinii;
    for (int j = 0; j < templates[i].numarlinii; j++)
        fin >> templates[i].forma[j].delta1.x >> templates[i].forma[j].delta1.y >> templates[i].forma[j].delta2.x >> templates[i].forma[j].delta2.y;
    fin >> templates[i].numarcercuri;
    for (int j = 0; j < templates[i].numarcercuri; j++)
        fin >> templates[i].cercuri[j].delta.x >> templates[i].cercuri[j].delta.y >> templates[i].cercuri[j].startg>>templates[i].cercuri[j].endg>>templates[i].cercuri[j].raza;
}

void ListaComponente() {
    auto latura = l * 0.05 + 10;
    char filename[50][50] = {
        "componente/back.jpg",
        "componente/DIODA.txt",
        "componente/BATERIE.txt",
        "componente/TRANZNPN.txt",
        "componente/TRANZPNP.txt",
        "componente/BEC.txt",
        "componente/CONDENS.txt",
        "componente/REZIST.txt",
        "componente/AND.txt",
        "componente/X.jpg"
    };
    setlinestyle(SOLID_LINE, 0, 3); //grosime linie
    for (auto i = 0, j = 50; i < nrTemplates; i++, j += latura) {
        if (i == 0 || i == nrTemplates-1)
            readimagefile(filename[i], 50, j, 150, j + latura); //pt back si deselect
        else
        {
            LoadFileComponenta(filename[i], i); //citeste descriere componenta
            templates[i].centru.x = 100;
            templates[i].centru.y = j + latura / 2;
            DeseneazaComponenta(templates[i]);
        }       
        rectangle(50, j, 150, latura + j); //outline forma
        templates[i].id = i;
    }
    setlinestyle(SOLID_LINE, 0, 1);
}

void SchemaElectronica() //asta o modific direct, nu cred ca are nevoie de multe schimbari
{
    ListaComponente();
    if (paginaVeche == true) //incarca pagina veche
    {
        char nume[50] = "test.txt";
        LoadFile(nume);
    }
    DeseneazaLegatura();
    while (!inMeniu) {
        int x, y;
        CoordonateCursorClick(x, y);
        float buton = ApasaButon(x, y);
        if (deselect == true)
        {
            if (x >= margine_stanga && x <= margine_stanga + 80 * nrCol && y >= margine_sus && y <= margine_sus + 80 * nrLin)
                ApasaPin(x, y);
        }
        if (buton == 0)
        {
            cleardevice();
            inMeniu = true;
            SaveFile("test.txt");
            nrLegaturi = 0;
            break;
        }
        else if (buton == nrTemplates - 1)
        {
            deselect = true;
            setcolor(culoare_fundal);
            outtextxy(l / 2 - 50, 50, "ACUM PUI PIESE");
            setcolor(WHITE);
        }
        else if (buton != -1)
        {
            setcolor(LIGHTBLUE);
            outtextxy(l / 2 - 50, 50, "ACUM PUI PIESE");
            setcolor(WHITE);
            deselect = false;
            PuneComponenta(buton);
        }
    }
}

void Meniu() //la asta nu umblu
{
    while (1)
    {
        rectangle(100, 100, 500, 300); //buton start
        rectangle(100, 400, 500, 600); //buton exit
        rectangle(600, 100, 700, 300); //buton load file
        outtextxy(200, 200, "START");
        outtextxy(625, 150, "LOAD FILE");
        outtextxy(200, 500, "EXIT");
        int x, y;
        deselect = true;
        CoordonateCursorClick(x, y);
        if (x >= 100 && x <= 500 && y >= 100 && y <= 300) //verifica daca apasa start
        {
            cleardevice();
            inMeniu = false;
            paginaVeche = false;
            SchemaElectronica();//deschide urmatoarea pagina
        }
        if (x >= 600 && x <= 700 && y >= 100 && y <= 300) //verifica daca apasa load file
        {
            cleardevice();
            inMeniu = false;
            paginaVeche = true;
            SchemaElectronica();

        }
        if (x >= 100 && x <= 500 && y >= 400 && y <= 600) //verifica daca apasa exit
        {
            closegraph();
            break;
        }
    }
}

void initializareLista() {
    // DE FACUT +++++++++++++++++
}

int main()
{
    initwindow(l, h, "electron");
    setbkcolor(culoare_fundal);

    cleardevice();

    Meniu();

    closegraph();
    return 0;
}
