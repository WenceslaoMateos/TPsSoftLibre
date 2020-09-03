/*  CORRE SOBRE LINUX  */
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <dirent.h>
#define TAM_DIC 200
#define TAM_PAL 20
#define TAM_NOM_ARCH 20
#define TAM_NOM_DIR 20

typedef char stringarch[TAM_NOM_ARCH + TAM_NOM_DIR];

typedef stringarch *truta;

typedef char stringpal[TAM_PAL];

typedef struct tipoPosting
{
    unsigned short int D, F;
    short int PR;
} tipoPosting;

typedef struct paldic
{
    stringpal pal;
    unsigned short int TD, FT, RR;
} paldic;

typedef struct diccionario
{
    paldic *palabra;
    unsigned short int cantpal;
    unsigned short int cantmax;
} diccionario;

void parsing(char *file, diccionario *dic, unsigned short int numdir);
void generaDicc(diccionario *dic, truta *rutas, char *ruta);
void burbuja(paldic Vec[], unsigned int inicio, unsigned int fin);
short int strcompare(char *pal1, char *pal2);
void agrupamiento(diccionario *dic);
void dicPost(diccionario *dic, tipoPosting post[]);
void muestra(diccionario *dic, truta rutas, tipoPosting post[]);
int busqueda(diccionario *dic, tipoPosting post[], stringpal palabra);
void generaFin(diccionario *dic, truta rutas, tipoPosting post[]);
void menu(diccionario dic, tipoPosting post[], truta rutas);
void quickSort(paldic A[], int inf, int sup);
int particion(paldic arr[], int inf, int sup);

int main(int argsc, char *args1[])
{
    truta rutas;
    stringarch ruta;
    tipoPosting post[TAM_DIC];
    diccionario dic;
    dic.cantmax = TAM_DIC;
    dic.palabra = (paldic *) malloc(dic.cantmax * sizeof(paldic));
    strcpy(ruta, "./");
    if(argsc == 2)
        strcpy(ruta, args1[1]);
    generaDicc(&dic, &rutas, ruta);
    dic.palabra = (paldic *) realloc(dic.palabra, dic.cantpal * sizeof(paldic));
    if(dic.cantpal != 0)
    {
        printf("Parseando ... 100%c listo\n", '%');
        burbuja(dic.palabra, 0, dic.cantpal);
        printf("Ordenando ... 100%c listo\n", '%');
        agrupamiento(&dic);
        dic.palabra = (paldic *) realloc(dic.palabra, dic.cantpal * sizeof(paldic));
        printf("Agrupando ... 100%c listo\n", '%');
        dicPost(&dic, post);
        dic.palabra = (paldic *) realloc(dic.palabra, dic.cantpal * sizeof(paldic));
        printf("Generando diccionario y posteando ... 100%c listo\n", '%');
        //menu(dic, post, rutas);
        generaFin(&dic, rutas, post);
    }
    return 0;
}

void menu(diccionario dic, tipoPosting post[], truta rutas)
{
    char op;
    int j, encontrado;
    stringpal palabra;
    op = 's';
    while(toupper(op) != 'N')
    {
        printf("Ingrese la palabra a buscar :");
        scanf("%s", palabra);
        for(j = 0; strlen(palabra) < j; j++)
            palabra[j] = tolower(palabra[j]);
        encontrado = busqueda(&dic, post, palabra);
        if(encontrado != -1)
        {
            j = dic.palabra[encontrado].RR;
            printf("\tEncontrada en el archivo : %s , %hu veces\n", rutas[post[j].D], post[j].F);
            while(post[j].PR != -1)
            {
                j = post[j].PR;
                printf("\tEncontrada en el archivo : %s , %hu veces\n", rutas[post[j].D], post[j].F);
            }
        }
        else
            printf("No se encontro\n");
        printf("Buscamos otra palabra? (S/N) ");
        op = getchar();
        op = getchar();
    }
}

void generaFin(diccionario *dic, truta rutas, tipoPosting post[])
{
    int i, j;
    FILE *arch;
    arch = fopen("diccionario.tex", "wt");
    for(i = 0; i < dic->cantpal; i++){
        fprintf(arch, "%s TD#: %d FT#: %d RR#: %d\n", dic->palabra[i].pal, dic->palabra[i].TD, dic->palabra[i].FT, dic->palabra[i].RR);
        printf(arch, "%s TD#: %d FT#: %d RR#: %d\n", dic->palabra[i].pal, dic->palabra[i].TD, dic->palabra[i].FT, dic->palabra[i].RR);
    }
    fclose(arch);
    arch = fopen("posting.tex", "wt");
    j = 0;
    for(i = 0; i < dic->cantpal; i++)
    {
        j = dic->palabra[i].RR;
        fprintf(arch, "\tRR: %d Archivo: %s F#: %hu PR#: %d\n", j, rutas[post[j].D], post[j].F, post[j].PR);
        printf(arch, "\tRR: %d Archivo: %s F#: %hu PR#: %d\n", j, rutas[post[j].D], post[j].F, post[j].PR);
        while(post[j].PR != -1)
        {
            j = post[j].PR;
            fprintf(arch, "\tRR: %d Archivo: %s F#: %hu PR#: %d\n", j, rutas[post[j].D], post[j].F, post[j].PR);
            printf(arch, "\tRR: %d Archivo: %s F#: %hu PR#: %d\n", j, rutas[post[j].D], post[j].F, post[j].PR);
        }
    }
    fclose(arch);
}

int busqueda(diccionario *dic, tipoPosting post[], stringpal palabra)
{
    int inf, sup, centro;
    inf = 0;
    sup = dic->cantpal - 1;
    centro = ((sup - inf) / 2) + inf;
    while(inf <= sup && strcmp(dic->palabra[centro].pal, palabra) != 0)
    {
        if(strcmp(dic->palabra[centro].pal, palabra) > 0)
            sup = centro - 1;
        else
            inf = centro + 1;
        centro = ((sup - inf) / 2) + inf;
    }
    if(strcmp(dic->palabra[centro].pal, palabra) == 0)
        return centro;
    else
        return -1;
}

void muestra(diccionario *dic, truta rutas, tipoPosting post[])
{
    int i, j;
    for(i = 0; i < dic->cantpal; i++)
    {
        printf("Palabra: %s TotArch: %hu FrecTotal: %hu RR: %d\n", dic->palabra[i].pal, dic->palabra[i].TD, dic->palabra[i].FT, dic->palabra[i].RR);
        j = dic->palabra[i].RR;
        printf("\tArchivo: %s Frecuencia: %hu PR: %d\n", rutas[post[j].D], post[j].F, post[j].PR);
        while(post[j].PR != -1)
        {
            j = post[j].PR;
            printf("\tArchivo: %s Frecuencia: %hu PR: %d\n", rutas[post[j].D], post[j].F, post[j].PR);
        }
    }
}

void dicPost(diccionario *dic, tipoPosting post[])
{
    short int i, k;
    k = 0; //finaliza el diccionario
    //al arrancar, agarra el primero y lo procesa
    post[0].D = dic->palabra[k].TD;
    post[0].F = dic->palabra[k].FT;
    post[0].PR = -1;
    dic->palabra[0].TD = 1;
    dic->palabra[0].RR = 0;
    for(i = 1; i < dic->cantpal; i++)
    {
        if(strcmp(dic->palabra[i].pal, dic->palabra[k].pal) == 0) //encuentra 2 palabras iguales en diccionario
        {
            dic->palabra[k].FT += dic->palabra[i].FT;
            dic->palabra[k].TD++;
            post[i - 1].PR = i;
            post[i].D = dic->palabra[i].TD;
            post[i].F = dic->palabra[i].FT;
        }
        else
        {
            k++;
            post[i].D = dic->palabra[i].TD;
            post[i].F = dic->palabra[i].FT;
            strcpy(dic->palabra[k].pal, dic->palabra[i].pal);
            dic->palabra[k].FT = dic->palabra[i].FT;
            dic->palabra[k].TD = 1;
            dic->palabra[k].RR = i;
        }
        post[i].PR = -1;
    }
    dic->cantpal = k;
}

void agrupamiento(diccionario *dic)
{
    unsigned short int i, j;
    i = 1;
    j = 0;
    while(i < dic->cantpal)
    {
        if(strcmp(dic->palabra[i].pal, dic->palabra[j].pal) == 0 && dic->palabra[i].TD == dic->palabra[j].TD)
            (dic->palabra[j].FT)++;
        else
        {
            j++;
            dic->palabra[j].TD = dic->palabra[i].TD;
            strcpy(dic->palabra[j].pal, dic->palabra[i].pal);
            dic->palabra[j].FT = dic->palabra[i].FT;
        }
        i++;
    }
    dic->cantpal = j++;
}

void generaDicc(diccionario *dic, truta *rutas, char *ruta)
{
    char file[TAM_NOM_ARCH];
    /* Con un puntero a DIR abriremos el directorio */
    DIR *dir;
    /* en *ent habrá información sobre el archivo que se está "sacando" a cada momento */
    struct dirent *ent;
    unsigned short int numdir = 0, numax = TAM_DIC;
    /* Empezaremos a leer en el directorio actual */
    *rutas = (truta)malloc(20 * sizeof(stringarch));
    dir = opendir(ruta);
    dic->cantpal = 0;
    /* Miramos que no haya error */
    if(dir == NULL)
        printf("No se pudo abrir el directorio\n");
    else
    {
        /* Leyendo uno a uno todos los archivos que hay */
        while((ent = readdir(dir)) != NULL)
        {
            strcpy(file, ent->d_name);
            if(strcmp(file + strlen(file) - 4, ".txt") == 0)
            {
                strcpy((*rutas)[numdir], ruta);
                strcat((*rutas)[numdir], file);
                parsing((*rutas)[numdir], dic, numdir);
                numdir++;
                if(numdir >= numax)
                {
                    numax += TAM_DIC;
                    *rutas = (truta)realloc((*rutas), (numdir + 1) * sizeof(stringarch));
                }
            }
        }
        *rutas = (truta)realloc((*rutas), (numdir + 1) * sizeof(stringarch));
        printf("%d archivo(s) encontrado(s)\n", numdir);
    }
    if(numdir == 0)
    {
        printf("El directorio estavacio\n");
    }
    closedir(dir);
    //    printf("%s %s",rutas[0],rutas[1]);
}

void parsing(char *file, diccionario *dic, unsigned short int numdir)
{
    FILE *arch;
    char aux[TAM_PAL];
    unsigned short int i;
    arch = fopen(file, "rt");
    while(!feof(arch))
    {
        i = 0;
        fscanf(arch, "%c", &(aux[i]));
        aux[i] = tolower(aux[i]);
        while(aux[i] <= 122 && aux[i] >= 97 && !feof(arch))
        {
            i++;
            fscanf(arch, "%c", &(aux[i]));
            aux[i] = tolower(aux[i]);
        }
        if(i != 0)
        {
            aux[i] = '\0';
            dic->cantpal++;
            if(dic->cantpal >= dic->cantmax)
            {
                dic->cantmax += TAM_DIC;
                dic->palabra = (paldic *) realloc(dic->palabra, dic->cantmax * sizeof(paldic));
            }
            strcpy((dic->palabra[(dic->cantpal - 1)].pal), aux);
            dic->palabra[(dic->cantpal - 1)].TD = numdir;
            dic->palabra[(dic->cantpal - 1)].FT = 1;
        }
    }
    fclose(arch);
}

int particion(paldic arr[], int inf, int sup)
{
    int i = (inf - 1);
    paldic aux;
    for(int j = inf; j <= sup - 1; j++)
        if(strcmp(arr[j].pal, arr[sup].pal) > 0 || (strcmp(arr[j].pal, arr[sup].pal) == 0 && arr[j].TD > arr[sup].TD))
        {
            i++;
            aux = arr[i];
            arr[i] = arr[j];
            arr[j] = aux;
        }
    aux = arr[i + 1];
    arr[i + 1] = arr[sup];
    arr[sup] = aux;
    return (i + 1);
}

void quickSort(paldic A[], int inf, int sup)
{
    if(inf < sup)
    {
        int p = particion(A, inf, sup);
        quickSort(A, inf, p - 1);
        quickSort(A, p + 1, sup);
    }
}

void burbuja(paldic Vec[], unsigned int inicio, unsigned int fin)
{
    int i, j;
    paldic aux;
    for(i = 0; i < fin; i++)
        for(j = i; j < fin; j++)
            if(strcmp(Vec[i].pal, Vec[j].pal) > 0 || (strcmp(Vec[i].pal, Vec[j].pal) == 0 && Vec[i].TD > Vec[j].TD))
            {
                aux = Vec[i];
                Vec[i] = Vec[j];
                Vec[j] = aux;
            }
}
