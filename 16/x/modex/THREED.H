typedef struct {
    long    x, y, z;
} TVECTOR;

#define PVECTOR TVECTOR far *

#define TPOINT  TVECTOR
#define PPOINT  PVECTOR

#define VPTR    void far *

#ifdef __cplusplus
extern "C" {
#endif

long far pascal tdFixedMul( long, long );
int  far pascal tdGetSurfaceLight( PPOINT );
long far pascal tdTransformToImage( VPTR, VPTR, short, short, short );

void far pascal tdBackPlaneCull( VPTR, VPTR, VPTR, short, short );
void far pascal tdGetNormal( VPTR, PPOINT, PPOINT, PPOINT );
void far pascal tdRotate( VPTR, VPTR, short );
void far pascal tdSetLight( PVECTOR );
void far pascal tdSetRotation( short, short, short );
void far pascal tdSetTranslation( PVECTOR );
void far pascal tdSetPerspective( long, long, long );
void far pascal tdTransform( VPTR, VPTR, short );
void far pascal tdTransformLight( VPTR, VPTR, short );

#ifdef __cplusplus
}
#endif
