#include <gtk/gtk.h>

#define LargeurImage 360
#define CloudColor 180
#define CloudChannel 1

void ComputeImage(guchar *pucImaOrig, 
		  guint uiNbCol, 
		  guint uiNbLine,
		  guchar *pucImaRes);
