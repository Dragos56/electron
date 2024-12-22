#include "graphics.h"
#include "winbgim.h"
#include <windows.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <fstream>

using namespace std;

#define l getmaxwidth()
#define h getmaxheight()
#define margine_stanga 250
#define margine_sus 100
#define culoare_fundal RGB(17, 17, 17)
#define culoare_text RGB(60, 240, 255)
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
int paginaCurenta = 1;
char filename[50][50];

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
    float orientare = 0;
    float zoom;
    int id;
    int numarpini = 0;
    //bool apasaPin[MAX1] = { false };
    coordonate centru;
    coordonate pin[MAX1] = { (0,0) };
    int numarlinii = 0;
    linii forma[MAX2];
    int numarcercuri = 0;
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

void SchimbaPagina();
void CoordonateCursorClick();
void Legatura();
void DeseneazaComponente();
void DeseneazaPin();
void ZoomComponenta();
void RotesteComponenta();
void StergeComponenta();
void ApasaComponenta();
int ApasaButon();
int ApasaFunctie();
void ApasaPin();
void PuneComponenta();
void SaveFile();
void LoadFile();
void LoadFileComponenta();
void ListaButoane();
void ListaComponente();
void MiscaComponente();
void SchemaElectronica();
void Meniu();

void SchimbaPagina() {
    paginaCurenta = 3 - paginaCurenta;
    setactivepage(paginaCurenta);
}

void CoordonateCursorClick(int& x, int& y)
{
    while (!ismouseclick(WM_LBUTTONDOWN));
    getmouseclick(WM_LBUTTONDOWN, x, y);
    clearmouseclick(WM_LBUTTONDOWN);
}

void Legatura(float x1, float y1, float x2, float y2, float culoare)
{
    setcolor(culoare);
    line(x1, y1, (x1 + x2) / 2, y1);
    line((x1 + x2) / 2, y1, (x1 + x2) / 2, y2);
    line((x1 + x2) / 2, y2, x2, y2);
}

void DeseneazaComponenta(piesa componenta, bool EsteDinLista)
{
    setcolor(WHITE);
    coordonate centru = componenta.centru;
    for (int i = 0; i < componenta.numarlinii; i++)
    {
        float x1 = componenta.forma[i].delta1.x;
        float y1 = componenta.forma[i].delta1.y;
        float x2 = componenta.forma[i].delta2.x;
        float y2 = componenta.forma[i].delta2.y;
        float zoom = componenta.zoom;
        if (EsteDinLista == 1)
            zoom = 1;
        line(centru.x + x1 * zoom, centru.y + y1 * zoom, centru.x + x2 * zoom, centru.y + y2 * zoom);
    }
    for (int i = 0; i < componenta.numarcercuri; i++)
    {
        float x1 = componenta.cercuri[i].delta.x;
        float y1 = componenta.cercuri[i].delta.y;
        float s = componenta.cercuri[i].startg;
        float e = componenta.cercuri[i].endg;
        float r = componenta.cercuri[i].raza;
        float zoom = componenta.zoom;
        if (EsteDinLista == 1)
            zoom = 1;
        ellipse(centru.x + x1 * zoom, centru.y + y1 * zoom, s, e, r * zoom, r * zoom);
    }
}

void DeseneazaPin(piesa componenta)
{
    for (int j = 0; j < componenta.numarpini; j++)
    {
        float zoom = componenta.zoom;
        float px = componenta.pin[j].x * zoom + componenta.centru.x;
        float py = componenta.pin[j].y * zoom + componenta.centru.y;
        rectangle(px - 5, py - 5, px + 5, py + 5);
        setfillstyle(SOLID_FILL, WHITE);
        floodfill(px + 1, py + 1, WHITE);
    }
}

void DeseneazaLegatura()
{
    for (int i = 0; i < nrLegaturi; i++)
    {
        setcolor(WHITE);
        float zoom1 = Conexiuni[i].piesa1->zoom;
        float zoom2 = Conexiuni[i].piesa2->zoom;
        float coordonate1_x = Conexiuni[i].piesa1->pin[Conexiuni[i].pin1].x * zoom1 + Conexiuni[i].piesa1->centru.x;
        float coordonate1_y = Conexiuni[i].piesa1->pin[Conexiuni[i].pin1].y * zoom1 + Conexiuni[i].piesa1->centru.y;
        float coordonate2_x = Conexiuni[i].piesa2->pin[Conexiuni[i].pin2].x * zoom2 + Conexiuni[i].piesa2->centru.x;
        float coordonate2_y = Conexiuni[i].piesa2->pin[Conexiuni[i].pin2].y * zoom2 + Conexiuni[i].piesa2->centru.y;
        Legatura(coordonate1_x, coordonate1_y, coordonate2_x, coordonate2_y, WHITE);
    }
}

void ZoomComponenta(int k, int semn)
{
    if (componente[k].zoom < 2 && semn == 1)
        componente[k].zoom += 0.1;
    if (componente[k].zoom > 1 && semn == -1)
        componente[k].zoom -= 0.1;
}

void RotesteComponenta(int k)
{
    float x;
    float y;
    for (int i = 0; i < componente[k].numarpini; i++)
    {
        x = componente[k].pin[i].x;
        y = componente[k].pin[i].y;
        componente[k].pin[i].x = y;
        componente[k].pin[i].y = -x;
    }
    for (int i = 0; i < componente[k].numarlinii; i++)
    {
        x = componente[k].forma[i].delta1.x;
        y = componente[k].forma[i].delta1.y;
        componente[k].forma[i].delta1.x = y;
        componente[k].forma[i].delta1.y = -x;
        x = componente[k].forma[i].delta2.x;
        y = componente[k].forma[i].delta2.y;
        componente[k].forma[i].delta2.x = y;
        componente[k].forma[i].delta2.y = -x;
    }
    for (int i = 0; i < componente[k].numarcercuri; i++)
    {
        componente[k].cercuri[i].startg += +90;
        componente[k].cercuri[i].endg += +90;
        if (componente[k].cercuri[i].startg >= 360)
            componente[k].cercuri[i].startg -= 360;
        if (componente[k].cercuri[i].endg >= 360)
            componente[k].cercuri[i].endg -= 360;
        x = componente[k].cercuri[i].delta.x;
        y = componente[k].cercuri[i].delta.y;
        componente[k].cercuri[i].delta.x = y;
        componente[k].cercuri[i].delta.y = -x;
    }
}

void StergeComponenta(int k) {
    // stergem componenta
    for (int i = k; i < nrComponente; i++) {
        componente[i] = componente[i + 1];
    }
    nrComponente--;
    // stergem legaturile
    for (int i = 0; i < nrLegaturi; i++) {
        int p1 = Conexiuni[i].pozitie1;
        int p2 = Conexiuni[i].pozitie2;
        if (p1 == k || p2 == k) {
            for (int j = i; j < nrLegaturi; j++) {
                Conexiuni[j] = Conexiuni[j + 1];
            }
            nrLegaturi--;
            i--;
        }
        else {
            if (p1 > k) {
                Conexiuni[i].pozitie1--;
                Conexiuni[i].piesa1 = &componente[Conexiuni[i].pozitie1];
            }
            if (p2 > k) {
                Conexiuni[i].pozitie2--;
                Conexiuni[i].piesa2 = &componente[Conexiuni[i].pozitie2];
            }
        }
    }
}

void ApasaComponenta(float x, float y, int& pozitie)
{
    for (int i = 0; i < nrComponente; i++)
    {
        float x1 = componente[i].centru.x;
        float y1 = componente[i].centru.y;
        if (x >= x1 - 25 && x <= x1 + 25 && y >= y1 - 25 && y <= y1 + 25)
        {
            pozitie = i;
            return;
        }
    }
}

int ApasaButon(float x, float y)
{
    auto latura = l / (nrTemplates * 2);
    for (float i = 0; i <= 9; i++)
    {
        if (x >= 50 && x <= 150 && y >= (50 + latura * i) && y <= (50 + latura * (i + 1)))
            return i;
    }
    return -1;
}

int ApasaFunctie(float x, float y, int k)
{
    float centru_x = componente[k].centru.x;
    float centru_y = componente[k].centru.y;
    if (x >= centru_x - 80 && x <= centru_x - 40 && y <= centru_y - 80 && y >= centru_y - 130)
        return 1;
    if (x >= centru_x - 39 && x <= centru_x && y <= centru_y - 80 && y >= centru_y - 130)
        return 2;
    if (x >= centru_x + 1 && x <= centru_x + 40 && y <= centru_y - 80 && y >= centru_y - 130)
        return 3;
    if (x >= centru_x + 41 && x <= centru_x + 80 && y <= centru_y - 80 && y >= centru_y - 130)
        return 4;
    if (x >= centru_x - 20 && x <= centru_x + 20 && y <= centru_y + 20 && y >= centru_y - 20)
        return 5;
    return -1;
}

void ApasaPin(float x, float y) {
    for (int i = 0; i < nrComponente; i++)
    {
        for (int j = 0; j < componente[i].numarpini; j++)
        {
            float zoom = componente[i].zoom;
            float px = componente[i].pin[j].x * zoom + componente[i].centru.x;
            float py = componente[i].pin[j].y * zoom + componente[i].centru.y;
            if (x >= px - 5 && y >= py - 5 && x <= px + 5 && y <= py + 5)
            {
                if (drawing == false) {
                    setcolor(LIGHTBLUE);
                    outtextxy(l / 2 - 25, 50, "ACUM DESENEZI LEGATURI");
                    setcolor(WHITE);
                    Conexiuni[nrLegaturi].piesa1 = &componente[i];
                    Conexiuni[nrLegaturi].pin1 = j;
                    Conexiuni[nrLegaturi].pozitie1 = i;
                    drawing = true;
                    float a, b;
                    while (drawing) {
                        a = mousex();
                        b = mousey();
                        while (!ismouseclick(WM_LBUTTONDOWN)) {
                            //vezi cum faci sa desenezi constant, redesenezi tot
                            // facut?
                            float x = mousex();
                            float y = mousey();
                            SchimbaPagina();
                            cleardevice();
                            if (x > margine_stanga && x < l - 100 && y > margine_sus && y < h - 100) {
                                Legatura(px, py, x, y, WHITE);
                            }
                            for (int i = 0; i < nrComponente; i++)
                            {
                                DeseneazaComponenta(componente[i], 0);
                                DeseneazaPin(componente[i]);
                            }
                            ListaComponente();
                            DeseneazaLegatura();
                            setcolor(LIGHTBLUE);
                            outtextxy(l / 2 - 25, 50, "ACUM DESENEZI LEGATURI");
                            setcolor(WHITE);
                            setvisualpage(paginaCurenta);
                        }
                        clearmouseclick(WM_LBUTTONDOWN);
                        a = mousex();
                        b = mousey();
                        SchimbaPagina();
                        cleardevice();
                        for (int i = 0; i < nrComponente; i++)
                        {
                            DeseneazaComponenta(componente[i], 0);
                            DeseneazaPin(componente[i]);
                        }
                        ListaComponente();
                        DeseneazaLegatura();
                        setvisualpage(paginaCurenta);
                        if (ApasaButon(a, b) != -1)
                        {
                            drawing = false;
                            return;
                        }
                        ApasaPin(a, b);
                    }
                }
                else if (Conexiuni[nrLegaturi].piesa1 != &componente[i])
                {
                    Conexiuni[nrLegaturi].piesa2 = &componente[i];
                    Conexiuni[nrLegaturi].pin2 = j;
                    Conexiuni[nrLegaturi].pozitie2 = i;
                    setcolor(culoare_fundal);
                    outtextxy(l / 2 - 25, 50, "ACUM DESENEZI LEGATURI");
                    setcolor(WHITE);
                    nrLegaturi++;
                    SchimbaPagina();
                    cleardevice();
                    for (int i = 0; i < nrComponente; i++)
                    {
                        DeseneazaComponenta(componente[i], 0);
                        DeseneazaPin(componente[i]);
                    }
                    ListaComponente();
                    DeseneazaLegatura();
                    setvisualpage(paginaCurenta);
                    drawing = false;
                }
            }
        }
    }
}

void PuneComponenta(int k)
{
    int x, y;
    auto latura = l * 0.05;
    auto laturaBorder = l / (nrTemplates * 2);
    int x1, x2, y1, y2;
    x1 = 50;
    y1 = 50 + laturaBorder * k;
    x2 = 150;
    y2 = 50 + laturaBorder * (k + 1);
    setcolor(YELLOW);
    rectangle(x1, y1, x2, y2);
    setcolor(WHITE);
    while (1 && k != 0 && !inMeniu && !deselect)
    {
        CoordonateCursorClick(x, y);
        if (x > margine_stanga && x < l - 100 && y > margine_sus && y < h - 100)
        {
            componente[nrComponente] = templates[k];
            componente[nrComponente].centru.x = x;
            componente[nrComponente].centru.y = y;
            componente[nrComponente].zoom = 1.5;
            DeseneazaComponenta(componente[nrComponente], 0);
            DeseneazaPin(componente[nrComponente]);
            nrComponente++;
        }
        int k = ApasaButon(x, y);
        if (k == 0)
        {
            setvisualpage(3);
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
            rectangle(x1, y1, x2, y2);
        }
        else if (k != -1)
        {
            setcolor(LIGHTBLUE);
            outtextxy(l / 2 - 50, 50, "ACUM PUI PIESE");
            setcolor(WHITE);
            rectangle(x1, y1, x2, y2);
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
        fout << componente[i].id << " " << componente[i].centru.x << " " << componente[i].centru.y << " " << componente[i].zoom << " " << componente[i].orientare << endl;
    }
    fout << nrLegaturi << endl;
    for (int i = 0; i < nrLegaturi; i++)
    {
        fout << Conexiuni[i].pozitie1 << " " << Conexiuni[i].pin1 << " " << Conexiuni[i].pozitie2 << " " << Conexiuni[i].pin2 << endl;
    }
    fout.close();
}

void LoadFile(char nume[50])
{
    ifstream fin(nume);
    fin >> nrComponente;
    for (int i = 0; i < nrComponente; i++)
    {
        fin >> componente[i].id;
        componente[i] = templates[componente[i].id];
        fin >> componente[i].centru.x >> componente[i].centru.y >> componente[i].zoom >> componente[i].orientare;
        int aux = componente[i].orientare;
        while (aux != 0)
        {
            RotesteComponenta(i);
            aux--;
        }
        DeseneazaComponenta(componente[i], 0);
        DeseneazaPin(componente[i]);
    }
    fin >> nrLegaturi;
    for (int i = 0; i < nrLegaturi; i++)
    {
        fin >> Conexiuni[i].pozitie1 >> Conexiuni[i].pin1 >> Conexiuni[i].pozitie2 >> Conexiuni[i].pin2;
        Conexiuni[i].piesa1 = &componente[Conexiuni[i].pozitie1];
        Conexiuni[i].piesa2 = &componente[Conexiuni[i].pozitie2];
    }
    fin.close();
}

void LoadFileComponenta(char nume[50], int i)
{
    ifstream fin(nume);
    fin >> templates[i].numarpini;
    for (int j = 0; j < templates[i].numarpini; j++)
        fin >> templates[i].pin[j].x >> templates[i].pin[j].y;
    fin >> templates[i].numarlinii;
    for (int j = 0; j < templates[i].numarlinii; j++)
        fin >> templates[i].forma[j].delta1.x >> templates[i].forma[j].delta1.y >> templates[i].forma[j].delta2.x >> templates[i].forma[j].delta2.y;
    fin >> templates[i].numarcercuri;
    for (int j = 0; j < templates[i].numarcercuri; j++)
        fin >> templates[i].cercuri[j].delta.x >> templates[i].cercuri[j].delta.y >> templates[i].cercuri[j].startg >> templates[i].cercuri[j].endg >> templates[i].cercuri[j].raza;
}

void ListaButoane(int k)
{
    setcolor(WHITE);
    float x = componente[k].centru.x;
    float y = componente[k].centru.y;
    rectangle(x - 80, y - 80, x + 80, y - 130);
    line(x - 40, y - 80, x - 40, y - 130);
    line(x, y - 80, x, y - 130);
    line(x + 40, y - 80, x + 40, y - 130);

    readimagefile("componente/roteste.jpg", x - 79, y - 129, x - 41, y - 81);//roteste

    line(x - 30, y - 105, x - 10, y - 105); //+
    line(x - 20, y - 90, x - 20, y - 120);

    line(x + 30, y - 105, x + 10, y - 105); //-

    readimagefile("componente/gunoi.jpg", x + 41, y - 129, x + 79, y - 81); //gunoi
}

void ListaComponente() {
    auto latura = l / (nrTemplates * 2);
    for (auto i = 0, j = 50; i < nrTemplates; i++, j += latura) {
        if (i == 0)
            readimagefile(filename[i], 50, j, 150, j + latura); //pt back si deselect
        else
        {
            LoadFileComponenta(filename[i], i); //citeste descriere componenta
            templates[i].centru.x = 100;
            templates[i].centru.y = j + latura / 2;
            DeseneazaComponenta(templates[i], 1);
        }
        rectangle(50, j, 150, latura + j); //outline forma
        templates[i].id = i;
    }
}

void MiscaComponenta(int k) {
    while (!ismouseclick(WM_LBUTTONDOWN)) {
        int x = mousex();
        int y = mousey();
        if (x > margine_stanga && x < l - 100 && y > margine_sus && y < h - 100) {
            componente[k].centru.x = x;
            componente[k].centru.y = y;
        }
        SchimbaPagina();
        cleardevice();
        for (int i = 0; i < nrComponente; i++)
        {
            DeseneazaComponenta(componente[i], 0);
            DeseneazaPin(componente[i]);
        }
        ListaComponente();
        DeseneazaLegatura();
        setvisualpage(paginaCurenta);
    }
    int x, y;
    getmouseclick(WM_LBUTTONDOWN, x, y);
    clearmouseclick(WM_LBUTTONDOWN);
    if (x > margine_stanga && x < l - 100 && y > margine_sus && y < h - 100) {
        componente[k].centru.x = x;
        componente[k].centru.y = y;
    }
    SchimbaPagina();
    cleardevice();
    for (int i = 0; i < nrComponente; i++)
    {
        DeseneazaComponenta(componente[i], 0);
        DeseneazaPin(componente[i]);
    }
    ListaComponente();
    DeseneazaLegatura();
    setvisualpage(paginaCurenta);
}

void SchemaElectronica() //asta o modific direct, nu cred ca are nevoie de multe schimbari
{
    SchimbaPagina();
    cleardevice();
    ListaComponente();
    if (paginaVeche == true) //incarca pagina veche
    {
        char nume[50] = "test.txt";
        LoadFile(nume);
    }
    DeseneazaLegatura();
    setvisualpage(paginaCurenta);
    while (!inMeniu) {
        int x, y;
        CoordonateCursorClick(x, y);
        float buton = ApasaButon(x, y);
        if (deselect == true) // verifica daca apasa un pin doar daca nu plasam piese
        {
            if (x >= margine_stanga && x <= l - margine_stanga && y >= margine_sus && y <= h - margine_sus)
                ApasaPin(x, y);
        }
        if (buton == 0) // butonul care duce inapoi in meniu
        {
            setvisualpage(3);
            cleardevice();
            inMeniu = true;
            SaveFile("test.txt");
            nrLegaturi = 0;
            nrComponente = 0;
            break;
        }
        else if (buton == nrTemplates - 1) // butonul de deselectare
        {
            deselect = true;
            setcolor(culoare_fundal);
            outtextxy(l / 2 - 50, 50, "ACUM PUI PIESE");
            setcolor(WHITE);
        }
        else if (buton != -1) // componentele
        {
            setcolor(LIGHTBLUE);
            outtextxy(l / 2 - 50, 50, "ACUM PUI PIESE");
            setcolor(WHITE);
            deselect = false;
            PuneComponenta(buton);
        }
        int pozitie = -1;
        ApasaComponenta(x, y, pozitie);
        if (pozitie != -1) // apasam pe o componenta
        {
            ListaButoane(pozitie); // deschidem meniul 
            CoordonateCursorClick(x, y);
            int buton = ApasaFunctie(x, y, pozitie);
            while (buton != -1)
            {
                switch (buton)
                {
                case 1:
                    componente[pozitie].orientare++;
                    if (componente[pozitie].orientare > 3)
                        componente[pozitie].orientare = 0;
                    RotesteComponenta(pozitie); break;
                case 2:ZoomComponenta(pozitie, 1); break;
                case 3:ZoomComponenta(pozitie, -1); break;
                case 4:StergeComponenta(pozitie); goto get_out;
                case 5:MiscaComponenta(pozitie); break;
                }
                SchimbaPagina();
                cleardevice();
                ListaComponente();
                for (int i = 0; i < nrComponente; i++)
                {
                    DeseneazaComponenta(componente[i], 0);
                    DeseneazaPin(componente[i]);
                }
                DeseneazaLegatura();
                ListaButoane(pozitie);
                setvisualpage(paginaCurenta);
                SchimbaPagina();
                cleardevice();
                SchimbaPagina();
                CoordonateCursorClick(x, y);
                buton = ApasaFunctie(x, y, pozitie);
                cout << buton;
            }
        get_out:
            SchimbaPagina();
            cleardevice();
            ListaComponente();
            for (int i = 0; i < nrComponente; i++)
            {
                DeseneazaComponenta(componente[i], 0);
                DeseneazaPin(componente[i]);
            }
            DeseneazaLegatura();
            SchimbaPagina();
            cleardevice();
            SchimbaPagina();
            setvisualpage(paginaCurenta);
        }
    }
}

void Meniu() //la asta nu umblu
{
    while (1)
    {
        setactivepage(3);
        setvisualpage(3);
        readimagefile("componente/background.jpg", -15, 0, getmaxwidth(), getmaxheight());
        setcolor(culoare_text);
        setbkcolor(BLACK);
        outtextxy(l / 4 + 250, h - 100, "PROIECT REALIZAT DE FRON DRAGOS-GABRIEL SI STOEAN STEFAN");
        setbkcolor(culoare_fundal);
        setcolor(WHITE);
        //rectangle(l/2-230, 440, l/2+230, 550); //buton start
        //rectangle(l / 2 - 230, 615, l / 2 + 230, 725); //buton exit
        //rectangle(l/2-160 , 780, l/2+160, 890); //buton load file  
        int x, y;
        deselect = true;
        CoordonateCursorClick(x, y);
        if (x >= l / 2 - 230 && x <= l / 2 + 230 && y >= 440 && y <= 550) //verifica daca apasa start
        {
            setactivepage(paginaCurenta);
            setvisualpage(paginaCurenta);
            inMeniu = false;
            paginaVeche = false;
            nrComponente = 0;
            nrLegaturi = 0;
            SchemaElectronica();//deschide urmatoarea pagina
        }
        if (x >= l / 2 - 230 && x <= l / 2 + 230 && y >= 615 && y <= 725) //verifica daca apasa load file
        {
            setactivepage(paginaCurenta);
            setvisualpage(paginaCurenta);
            inMeniu = false;
            paginaVeche = true;
            SchemaElectronica();

        }
        if (x >= l / 2 - 160 && x <= l / 2 + 160 && y >= 780 && y <= 890) //verifica daca apasa exit
        {
            closegraph();
            break;
        }
    }
}

void InitializareLista() {
    ifstream f("componente/.Lista.txt");
    for (int i = 0; i < nrTemplates; i++) {
        f.getline(filename[i], 50);
    }
    f.close();
}

int main()
{
    initwindow(l, h, "Electron");

    InitializareLista();

    SchimbaPagina();

    setactivepage(paginaCurenta);
    setvisualpage(paginaCurenta);

    setbkcolor(culoare_fundal);
    cleardevice();

    SchimbaPagina();

    setactivepage(paginaCurenta);
    setvisualpage(paginaCurenta);

    setbkcolor(culoare_fundal);
    cleardevice();

    Meniu();

    closegraph();
    return 0;
}
