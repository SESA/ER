#ifdef HAS_VTK

#include <irtkImage.h>

char *input_name = NULL;
char *out_name = NULL;

void usage()
{
	cerr << "Usage: cardiacchecklandmarks [input] [number of correct landmarks] [output]" << endl;
	cerr << "this app checks if the number of landmarks are as specified, if so output will contain 1 else 0"    << endl;
	exit(1);
}

int main( int argc, char** argv )
{
	int ok,size;
	// Check command line
	if (argc < 4) {
		usage();
	}

	// Parse source and target images
	input_name = argv[1];
	argc--;
	argv++;
	size = atoi(argv[1]);
	argc--;
	argv++;
	out_name = argv[1];
	argc--;
	argv++;

	irtkPointSet input;
	// Read point set
	input.ReadVTK(input_name);

	ok = false;

	if(input.Size() == size){
		ok = true;
	}

	cout << "Landmark checked: " << ok << endl;
	ofstream fout(out_name,ios::app);
	fout << ok <<endl;
	fout.close();
}

#else

#include <irtkImage.h>

int main( int argc, char *argv[] )
{
  cerr << argv[0] << " needs to be compiled with the VTK library " << endl;
}
#endif