/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2008 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

  =========================================================================*/

/*
 * Edit a dof file from rigid/affine transformation.
 */

#include <irtkImage.h>
#include <irtkTransformation.h>

void usage()
{
  cerr << "Usage: dofedit dofin dofout <options>\n" << endl;
  cerr << "Edit a dof file for rigid/affine transformation." << endl;
  cerr << endl;

  cerr << "<options> can be one or more of the following:\n" << endl;
  cerr << "<-tx value>     Translation along the x-axis (in mm)" << endl;
  cerr << "<-ty value>     Translation along the y-axis (in mm)" << endl;
  cerr << "<-tz value>     Translation along the z-axis (in mm)" << endl;
  cerr << "<-rx value>     Rotation along the x-axis (in degrees)" << endl;
  cerr << "<-ry value>     Rotation along the y-axis (in degrees)" << endl;
  cerr << "<-rz value>     Rotation along the z-axis (in degrees)" << endl;
  cerr << "<-sx value>     Scaling along the x-axis" << endl;
  cerr << "<-sy value>     Scaling along the y-axis" << endl;
  cerr << "<-sz value>     Scaling along the z-axis" << endl;
  cerr << "<-sxy value>    Skew angle in the x direction based on y component (in degrees)" << endl;
  cerr << "<-syz value>    Skew angle in the y direction based on z component (in degrees)" << endl;
  cerr << "<-sxz value>    Skew angle in the x direction based on z component (in degrees)" << endl;
  cerr << endl;

  cerr << "<-add_tz image val>    Add more translation along the z-axis (in image voxels)" << endl;
  cerr << endl;
  exit(1);
}

int main(int argc, char *argv[])
{
  if(argc < 3){
    usage();
  }

  // Filename
  char *dofin_name = argv[1];
  argc--;
  argv++;
  char *dofout_name = argv[1];
  argc--;
  argv++;

  // Parameters
  bool ok;
  double tx = 0;
  double ty = 0;
  double tz = 0;
  double rx = 0;
  double ry = 0;
  double rz = 0;
  double sx = 100;
  double sy = 100;
  double sz = 100;
  double sxy = 0;
  double syz = 0;
  double sxz = 0;

  // Read the transformation
  irtkAffineTransformation *transformation = new irtkAffineTransformation;
  transformation->irtkTransformation::Read(dofin_name);

  // Parse the parameters
  while(argc > 1){
    ok = false;
    if((ok == false) && (strcmp(argv[1], "-tx") == 0)){
      argc--;
      argv++;
      tx = atof(argv[1]);
      transformation->PutTranslationX(tx);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-ty") == 0)){
      argc--;
      argv++;
      ty = atof(argv[1]);
      transformation->PutTranslationY(ty);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-tz") == 0)){
      argc--;
      argv++;
      tz = atof(argv[1]);
      transformation->PutTranslationZ(tz);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-rx") == 0)){
      argc--;
      argv++;
      rx = atof(argv[1]);
      transformation->PutRotationX(rx);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-ry") == 0)){
      argc--;
      argv++;
      ry = atof(argv[1]);
      transformation->PutRotationY(ry);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-rz") == 0)){
      argc--;
      argv++;
      rz = atof(argv[1]);
      transformation->PutRotationZ(rz);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-sx") == 0)){
      argc--;
      argv++;
      sx = atof(argv[1]);
      transformation->PutScaleX(sx);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-sy") == 0)){
      argc--;
      argv++;
      sy = atof(argv[1]);
      transformation->PutScaleY(sy);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-sz") == 0)){
      argc--;
      argv++;
      sz = atof(argv[1]);
      transformation->PutScaleZ(sz);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-sxy") == 0)){
      argc--;
      argv++;
      sxy = atof(argv[1]);
      transformation->PutShearXY(sxy);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-syz") == 0)){
      argc--;
      argv++;
      syz = atof(argv[1]);
      transformation->PutShearYZ(syz);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-sxz") == 0)){
      argc--;
      argv++;
      sxz = atof(argv[1]);
      transformation->PutShearXZ(sxz);
      argc--;
      argv++;
      ok = true;
    }
    if((ok == false) && (strcmp(argv[1], "-add_tz") == 0)){
      argc--;
      argv++;
      char *image_name = argv[1];
      argc--;
      argv++;
      double add_tz = atof(argv[1]);
      argc--;
      argv++;

      irtkGreyImage image(image_name);
      irtkImageAttributes attr = image.GetImageAttributes();
      tz = transformation->GetTranslationZ();
      tz += add_tz * attr._dz;
      transformation->PutTranslationZ(tz);
      ok = true;
    }
    if(ok == false){
      cerr << "Cannot parse argument " << argv[1] << endl;
      usage();
    }
  }

  // Write to file
  transformation->irtkTransformation::Write(dofout_name);

  return 0;
}
