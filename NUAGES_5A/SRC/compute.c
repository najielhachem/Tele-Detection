#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#define NbClass 10
#define VecSize 5

/*******************************************************
IL EST FORMELLEMENT INTERDIT DE CHANGER LE PROTOTYPE
DES FONCTIONS
*******************************************************/

int compare (const void* a,
		const void* b)
{
  return *(guchar*)b - *(guchar*)a;
}

int nearest(guchar* vector,
		guchar* means)
{
	int res = 0;
	int minDiff = VecSize * 255;

	/* Trouver la classe avec le vecteur moyen le plus proche selon la norme 1 */
	for (int i = 0; i < NbClass; ++i)
	{
		guchar diff = 0;
		for (int j = 0; j < VecSize; ++j)
			diff += abs(vector[j] - means[VecSize * i + j]);
		if (diff < minDiff)
		{
			minDiff = diff;
			res = i;
		}
	}
	return res;
}

void getVector(guchar* pucIm,
		int NbLine,
		int NbCol,
		int line,
		int col,
		guchar* vector)
{
	/* initialisation du vecteur a la valeur du pixel */
	for (int i = 0; i < VecSize; ++i)
		vector[i] = pucIm[NbCol * line + col];
	
	/* choisir la valeur du voisin si on n'est pas sur les bords */
	if (line > 1)
		vector[1] = pucIm[NbCol * (line - 1) + col];
	if (line < NbLine - 1)
		vector[2] = pucIm[NbCol * (line + 1) + col];
	if (col > 1)
		vector[3] = pucIm[NbCol * line + col - 1];
	if (col < NbCol - 1)
		vector[4] = pucIm[NbCol * line + col + 1];

	/* trie du vecteur */
	qsort(vector, VecSize, sizeof(guchar), compare);
}


/* Applique K-Means et retourne l'indice de la classe clouds */
int KMeans(guchar* pucIm,
		int NbLine,
		int NbCol,
		int* pClasses)
{
	/* declaration des moyennes des classes */
	guchar means[VecSize * NbClass];
	/* initialisation des moyennes sur l'axe homogene */
	for (int i = 0; i < NbClass; ++i)
		for (int j = 0; j < VecSize; ++j)
			means[VecSize * i + j] = i * 255 / NbClass;
	
	/* initialisation des classes de chaque pixel a -1 */
	for (int i = 0; i < NbLine * NbCol; ++i)
		pClasses[i] = -1;

	/* lancement de l'algorithme */
 	int stable = 0;
	while (!stable)
	{
		guchar newMeans[VecSize * NbClass]; /* nouvelles moyennes des classes */
		int nbElts[NbClass]; /* nombre d'elements dans chaque classe */
		/* initialisation des nouveaux centres et les nb d'elts a zero */
		for (int i = 0; i < NbClass; ++i)
		{
			nbElts[i] = 0;
			for (int j = 0; j < VecSize; ++j)
				newMeans[i * VecSize + j] = 0;
		}

		/* association de la classe la plus proche a chaque pixel */
		/* calcule des nouveaux centres en parallel */
		for (int i = 0; i < NbLine; ++i)
		{
			for (int j = 0; j < NbCol; ++j)
			{
				/* trouver la classe la plus proche au vecteur voisin du pixel */
				guchar vector[VecSize];
				getVector(pucIm, NbLine, NbCol, i, j, vector);
				int c = nearest(vector, means);
				pClasses[NbCol * i + j] = c;
		
				/* ajout du vecteur a la nouvelle moyenne de sa classe */
				for (int k = 0; k < VecSize; ++k)
					newMeans[VecSize * c + k] += vector[k];
				nbElts[c] += 1; /* incrementation du nombre d'elements dans la classe */
			}
		}

		printf("computing new means\n");
		stable = 1; /* supposont l'etat stable */
		/* mise a jour des moyennes */
		for (int i = 0; i < NbClass; ++i)
		{
			/* prendre la mediane comme nouvelle moyenne 
			 * suffit de trier le vecteur et prendre la valeur au milieur */
			
			/* trie du vecteur */
			qsort(newMeans + VecSize * i, VecSize, sizeof(guchar), compare);
			/* prendre la medianne comme valeur homogene */
			nbElts[i] = nbElts[i] == 0 ? 1 : nbElts[i];
			guchar val = newMeans[VecSize * i + (VecSize / 2)] / nbElts[i];
			if (val != means[VecSize * i])
				stable = 0;
			/* fixer la nouvelle valeur de la moyenne */
			for (int j = 0; j < VecSize; ++j)
				means[VecSize * i + j] = val;
		}
	}

	/* recherche de la classe clouds grace aux moyennes */
	int clouds = 0;
	guchar maxVal = -1;
	for (int i = 0; i < NbClass; ++i)
	{
		if (maxVal < means[VecSize * i])
		{
			maxVal = means[VecSize * i];
			clouds = i;
		}
	}

	printf("end of kmeans\n");

	return clouds;
}


/*---------------------------------------
  Proto: 

  
  But: 

  Entrees: 
          --->le tableau des valeurs des pixels de l'image d'origine
	      (les lignes sont mises les unes à la suite des autres)
	  --->le nombre de lignes de l'image,
	  --->le nombre de colonnes de l'image,
          --->le tableau des valeurs des pixels de l'image resultat
	      (les lignes sont mises les unes à la suite des autres)


  Sortie:

  Rem: 

  Voir aussi:

  ---------------------------------------*/
void ComputeImage(guchar* pucImaOrig, 
		  int NbLine,
		  int NbCol, 
		  guchar* pucImaRes)
{
  int iNbChannels = 3; /* on travaille sur des images couleurs*/
  guchar ucMeanPix;

  printf("Segmentation de l'image.... A vous!\n");
  
  for (int iNumPix = 0; iNumPix < NbCol * NbLine; ++iNumPix)
	{
    /* moyenne sur les composantes RVB */
    ucMeanPix = (unsigned char) 
			(
				(
				 *(pucImaOrig + iNumPix * iNbChannels    ) +
				 *(pucImaOrig + iNumPix * iNbChannels + 1) +
				 *(pucImaOrig + iNumPix * iNbChannels + 2) 
				) / 3
			);
    /* sauvegarde du resultat */
    for (int iNumChannel = 0; iNumChannel < iNbChannels; iNumChannel++)
      *(pucImaRes + iNumPix * iNbChannels + iNumChannel) = ucMeanPix;
  }

	/* application du k-means */
	int pClasses[NbLine * NbCol];
	int clouds = KMeans(pucImaRes, NbLine, NbCol, pClasses);

	printf("cloud: %d\n", clouds);

	
	 
	
	/* mettre les pixels du nuage en blancs */
  for (int iNumPix = 0; iNumPix < NbCol * NbLine; ++iNumPix)
	{
    /* sauvegarde du resultat */
    for (int iNumChannel = 0; iNumChannel < iNbChannels; iNumChannel++)
		{
			int i = iNumPix * iNbChannels + iNumChannel;
			printf("%d - %d\n", iNumPix, pClasses[iNumPix]);
      if (pClasses[iNumPix] == clouds)
				*(pucImaRes + i) = 255;
			else
				*(pucImaRes + i) = *(pucImaOrig + i); 
		}
	}

	
}



