
#ifndef CONFIG_H_
#define CONFIG_H_

//=========================//
// Algorithm Configurables //
//=========================//

// Flip between single/double
#ifndef SINGLE_PRECISION
	#define SINGLE_PRECISION 1
#endif

// Define global precision
#ifndef PRECISION
	#if SINGLE_PRECISION
		#define PRECISION float
	#else
		#define PRECISION double
	#endif
#endif


//=========================//
//        Structures       //
//=========================//

typedef struct Complex {
	PRECISION real;
	PRECISION imaginary;
} Complex;

typedef struct Source {
	PRECISION l;
	PRECISION m;
	PRECISION intensity;
} Source;

typedef struct Visibility {
	PRECISION u;
	PRECISION v;
	PRECISION w;
	Complex intensity;
} Visibility;

typedef struct Config {
	int numVisibilities;
	int numSources;
	char *source_file;
	char *vis_file;
	bool gpu_enabled;
	bool synthetic_sources;
	bool synthetic_visibilities;
	double min_u;
	double max_u;
	double min_v;
	double max_v;
	double distanceBetweenVis;
	int numVisAcrossU;
	int numVisAcrossV;
} Config;


//=========================//
//     Function Headers    //
//=========================//
void initConfig (Config *config);
void loadSources(Config *config, Source *sources);
void loadVisibilities(Config *config, Visibility *visibilities);
void performExtraction(Config *config, Source *sources, Visibility *visibilities);
void saveVisibilities(Config *config, Visibility *visibilities);
Complex complexMult(Complex z1, Complex z2);

#endif /* CONFIG_H_ */
