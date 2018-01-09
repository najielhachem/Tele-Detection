#include "compute.h"

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include <dirent.h> 
#include <limits.h>       //For PATH_MAX
#include <string.h>

void LoadAndComputeImage(char* filePath)
{
   int NbCol, NbLine;
	 gchar *pcFileName;
   guchar *pucImaOrig, *pucImaRes;
   GdkPixbuf *pGdkPixbufImaOrig,*pGdkPixbufImaRes;

	 pcFileName = (gchar*) filePath;
	
	 /* chargement en memoire de l'image */
	 pGdkPixbufImaOrig = gdk_pixbuf_new_from_file(pcFileName, NULL);
	 /* echec du chargement de l'image en memoire */
	 if (pGdkPixbufImaOrig == NULL)
	 {
		 printf("Impossible de charger le fichier %s\n", pcFileName);
		 return;
	 }
	 /* reussite du chargement de l'image en memoire */

	 /* copie du pixbuff original */
	 pGdkPixbufImaRes = gdk_pixbuf_copy(pGdkPixbufImaOrig);

   /* recuperation du nombre de lignes et de colonnes de l'image*/
   NbCol = gdk_pixbuf_get_width(pGdkPixbufImaOrig); 
   NbLine = gdk_pixbuf_get_height(pGdkPixbufImaOrig);
   /* recuperation du tableau des pixels de l'image originale */
   pucImaOrig = gdk_pixbuf_get_pixels(pGdkPixbufImaOrig);
   /* recuperation du tableau des pixels de l'image resultat */
   pucImaRes = gdk_pixbuf_get_pixels(pGdkPixbufImaRes);

   /* on realise le calcul sur l'image */
   ComputeImage(pucImaOrig, NbLine, NbCol, pucImaRes);

	 float cloud = 0.0;
	 /* analise de l'image resultante */
	 for (int iNumPix = 0; iNumPix < NbCol * NbLine; ++iNumPix)
	 {
		 int isCloud = 1;
		 /* recherche des pixels nuages */
		 for (int iNumChannel = 0; iNumChannel < 3; iNumChannel++)
		 {
			 int i = iNumPix * 3 + iNumChannel;
			 if (iNumChannel == CloudChannel && *(pucImaRes + i) != CloudColor)
					 isCloud = 0;
			 if (iNumChannel != CloudChannel && *(pucImaRes + i) != 0)
					 isCloud = 0;
		 }
		 if (isCloud)
			 ++cloud;
	 }
	 float percentage = 100 * cloud / (NbCol * NbLine);
	 printf("Image: %s\n\tPourcentage de nuage: %.*f %%\n", filePath, 2, percentage);
}


int main (int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s directory_path\n", argv[0]);
		exit(1);
	}
	
	/* lecture du repertoire passe en argument */
  DIR *d;
  struct dirent *dir;
  d = opendir(argv[1]);
  if (d) {
    while ((dir = readdir(d)) != NULL) {
			/* ne pas lire les fichiers . et .. */
			if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
				continue;
			/* creation du chemin relatif vers l'image */
			char buf[PATH_MAX + 1];
			/* copy du nom du repertoire dans le buffer */
      strcpy(buf, argv[1]); 
			/* ajout du / entre le repertoire et le fichier si absent */
			if (argv[1][strlen(argv[1]) - 1] != '/')
				strcat(buf, "/"); 
			/* ajout du nom du fichier au buffer */
			strcat(buf, dir->d_name); 
			/* analyse de l'image */
			LoadAndComputeImage(buf);
    }
    closedir(d);
  }
  return(0);
}
