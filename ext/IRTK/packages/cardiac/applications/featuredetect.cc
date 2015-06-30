/*=========================================================================

Library   : Image Registration Toolkit (IRTK)
Module    : $Id$
Copyright : Imperial College, Department of Computing
Visual Information Processing (VIP), 2008 onwards
Date      : $Date$
Version   : $Revision$
Changes   : $Author$

=========================================================================*/

#include <irtkImage.h>

#include <irtkGradientImageFilter.h>

#include <irtkGaussianBlurring.h>

#ifdef HAS_CONTRIB

#include <irtkEuclideanDistanceTransform.h>

char *input_name = NULL, *output_name = NULL;

void usage()
{
	cerr << "Usage: featuredetect [in] [out]" << endl;
	cerr << "Need at least one of the following: " << endl;
	cerr << "<-intensity>            Add intensity to the output features " << endl;
	cerr << "<-gradient>             Add gradient x y z to the output features" << endl;
	cerr << "<-spatial>              Add spatial x y z to the output features" << endl;
	cerr << "<-spatialimage image>   Add spatial x y z of the image to the output features (weighted by std of intensity)" << endl;
	cerr << "<-segmentation image>   Add distance transformation of the segmentation to the output features" << endl;
	cerr << "<-blur value>           Blur the image with sigma before" << endl;
	exit(1);
}

int main(int argc, char **argv)
{
	irtkRealImage input, spatialimage, segmentation, segmentA, segmentB, segoutA, segoutB;
	int intensity, gradient, spatial, spatiali, segment, ok;
	intensity = false;
	gradient = false;
	spatial = false;
	spatiali = false;
	segment = false;

	if (argc < 3) {
		usage();
	}

	input_name  = argv[1];
	argc--;
	argv++;
	output_name = argv[1];
	argc--;
	argv++;

	// Read input
	input.Read(input_name);

	// Parse remaining parameters
	while (argc > 1){
		ok = false;
		if ((ok == false) && (strcmp(argv[1], "-intensity") == 0)){
			argc--;
			argv++;
			intensity = true;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-gradient") == 0)){
			argc--;
			argv++;
			gradient = true;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-spatial") == 0)){
			argc--;
			argv++;
			spatial = true;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-spatialimage") == 0)){
			argc--;
			argv++;
			spatiali = true;
			spatialimage.Read(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-segmentation") == 0)){
			argc--;
			argv++;
			segment = true;
			segmentation.Read(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-blur") == 0)){
			argc--;
			argv++;
			if(atof(argv[1]) > 0){
				irtkGaussianBlurring<irtkRealPixel> gaussianBlurring(atof(argv[1]));
				gaussianBlurring.SetInput (&input);
				gaussianBlurring.SetOutput(&input);
				gaussianBlurring.Run();
			}
			argc--;
			argv++;
			ok = true;
		}

		if (ok == false){
			cerr << "Can not parse argument " << argv[1] << endl;
			usage();
		}
	} 

	if(intensity == false && gradient == false && spatial == false && segment == false){
		usage();
	}

	int numberoffeatures = 0;

	if(intensity == true)
		numberoffeatures++;
	if(gradient == true){
		if(input.GetZ() > 1)
			numberoffeatures += 3;
		else
			numberoffeatures += 2;
	}
	if(spatial == true){
		if(input.GetZ() > 1)
			numberoffeatures += 3;
		else
			numberoffeatures += 2;
	}
	if(spatiali == true){
		if(input.GetZ() > 1)
			numberoffeatures += 3;
		else
			numberoffeatures += 2;
	}

	double min,max;
	int count,labelcount = 0;

	if(segment == true){

		segmentation.GetMinMax(&min,&max);

		min = min + 1;

		for(int i = min; i <= max; i++){
			count = 0;
			for (int z = 0; z < segmentation.GetZ(); z++) {
				for (int y = 0; y < segmentation.GetY(); y++) {
					for (int x = 0; x < segmentation.GetX(); x++) {
						if (segmentation(x, y, z) == i) {
							count++;
						}				
					}
				}
			}
			if(count > 0){
				labelcount++;
			}
		}

		numberoffeatures += labelcount;
	}

	irtkRealImage output;
	irtkImageAttributes atr = input.GetImageAttributes();
	atr._t = numberoffeatures;

	output.Initialize(atr);

	int featureindex = 0;
	if(intensity == true){
		for(int k = 0; k < input.GetZ(); k++){
			for(int j = 0; j < input.GetY(); j++){
				for(int i = 0; i < input.GetX(); i++){
					output.Put(i,j,k,featureindex,input.Get(i,j,k));
				}
			}
		}
		featureindex++;
	}
	if(gradient == true){
		irtkGradientImageFilter<short> gradient(irtkGradientImageFilter<short>::GRADIENT_VECTOR);
		irtkGreyImage tmp = input;
		irtkGreyImage *targetgradient = new irtkGreyImage();
		gradient.SetInput (&tmp);
		gradient.SetOutput(targetgradient);
		gradient.SetPadding(-1);
		gradient.Run();
		for(int k = 0; k < input.GetZ(); k++){
			for(int j = 0; j < input.GetY(); j++){
				for(int i = 0; i < input.GetX(); i++){
					if(input.GetXSize() < 0.01)
						output.Put(i,j,k,featureindex,targetgradient->GetAsDouble(i,j,k,0)/500);
					else
						output.Put(i,j,k,featureindex,targetgradient->GetAsDouble(i,j,k,0)*2);

					if(input.GetXSize() < 0.01)
						output.Put(i,j,k,featureindex+1,targetgradient->GetAsDouble(i,j,k,1)/500);
					else
						output.Put(i,j,k,featureindex+1,targetgradient->GetAsDouble(i,j,k,1)*2);

					if(input.GetZ() > 1){
						if(input.GetZSize() < 0.01)
							output.Put(i,j,k,featureindex+2,targetgradient->GetAsDouble(i,j,k,2)/500);
						else
							output.Put(i,j,k,featureindex+2,targetgradient->GetAsDouble(i,j,k,2)*2);
					}
				}
			}
		}
		delete targetgradient;
		if(input.GetZ() > 1)
			featureindex += 3;
		else
			featureindex += 2;
	}

	if(spatial == true){
		for(int k = 0; k < input.GetZ(); k++){
			for(int j = 0; j < input.GetY(); j++){
				for(int i = 0; i < input.GetX(); i++){
					double x,y,z;
					x = i;
					y = j;
					z = k;
					output.Put(i,j,k,featureindex,x);
					output.Put(i,j,k,featureindex+1,y);
					if(input.GetZ() > 1)
						output.Put(i,j,k,featureindex+2,z);
				}
			}
		}
		if(input.GetZ() > 1)
			featureindex += 3;
		else
			featureindex += 2;
	}

	if(spatiali == true){

		double std = spatialimage.GetSD(0);
		double scalex, scaley, scalez;
		scalex = std / input.GetX();
		scaley = std / input.GetY();
		scalez = std / input.GetZ();
		cout << "normalized spatial coordinate, scale factor: x "
			<< scalex << " y " << scaley << " z " << scalez << endl;

		for(int k = 0; k < input.GetZ(); k++){
			for(int j = 0; j < input.GetY(); j++){
				for(int i = 0; i < input.GetX(); i++){
					double x,y,z;
					x = i;
					y = j;
					z = k;
					output.Put(i,j,k,featureindex,x*scalex);
					output.Put(i,j,k,featureindex+1,y*scaley);
					if(input.GetZ() > 1)
						output.Put(i,j,k,featureindex+2,z*scalez);
				}
			}
		}
		if(input.GetZ() > 1)
			featureindex += 3;
		else
			featureindex += 2;
	}

	if(segment == true){
		irtkEuclideanDistanceTransform<irtkRealPixel> * edt = new irtkEuclideanDistanceTransform<irtkRealPixel>
			(irtkEuclideanDistanceTransform<irtkRealPixel>::irtkDistanceTransform3D);

		irtkImageAttributes atr = segmentation.GetImageAttributes();
		atr._t = labelcount;

		labelcount = 0;
		for(int i = min; i <= max; i++){
			count = 0;
			for (int z = 0; z < segmentation.GetZ(); z++) {
				for (int y = 0; y < segmentation.GetY(); y++) {
					for (int x = 0; x < segmentation.GetX(); x++) {
						if (segmentation(x, y, z) == i) {
							count++;
						}				
					}
				}
			}
			if(count > 0){

				// Threshold image
				segmentA = segmentation;
				segmentB = segmentation;
				for (int z = 0; z < segmentation.GetZ(); z++) {
					for (int y = 0; y < segmentation.GetY(); y++) {
						for (int x = 0; x < segmentation.GetX(); x++) {
							if (segmentation(x, y, z) == i) {
								segmentA(x, y, z) = 1;
								segmentB(x, y, z) = 0;
							} else {
								segmentA(x, y, z) = 0;
								segmentB(x, y, z) = 1;
							}
						}
					}
				}

				// Calculate EDT
				cout << "Finding Euclidean distance transform." << endl;
				cout << "Doing outside DT" << endl;
				edt->SetInput (& segmentA);
				edt->SetOutput(& segoutA);
				edt->Run();
				cout << "Doing inside DT" << endl;
				edt->SetInput (& segmentB);
				edt->SetOutput(& segoutB);
				edt->Run();

				for (int z = 0; z < input.GetZ(); z++) {
					for (int y = 0; y < input.GetY(); y++) {
						for (int x = 0; x < input.GetX(); x++) {
							segoutA(x, y, z)  = sqrt(segoutA(x, y, z)) - sqrt(segoutB(x, y, z));
						}
					}
				}

				// Write image

				for (int z = 0; z < input.GetZ(); z++) {
					for (int y = 0; y < input.GetY(); y++) {
						for (int x = 0; x < input.GetX(); x++) {
							output.PutAsDouble(x,y,z,featureindex,segoutA.GetAsDouble(x,y,z));
						}
					}
				}

				featureindex ++;

			}
		}
	}

	// Write image
	output.Write(output_name);

	return 0;
}

#else

int main(int argc, char **argv)
{
	cerr << "Needs to be compiled with HAS_CONTRIB" << endl;
	return 0;
}

#endif