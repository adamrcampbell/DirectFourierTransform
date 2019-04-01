
#ifdef __cplusplus
extern "C" {
#endif

#ifndef DIRECT_FOURIER_TRANSFORM_H_
#define DIRECT_FOURIER_TRANSFORM_H_

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
	bool gaussian_distribution_sources;
	double min_u;
	double max_u;
	double min_v;
	double max_v;
	double grid_size;
	double cell_size;
	double uv_scale;
	double frequency_hz;
} Config;


//=========================//
//     Function Headers    //
//=========================//
void initConfig (Config *config);
void loadSources(Config *config, Source **sources);
void loadVisibilities(Config *config, Visibility **visibilities);
void performExtraction(Config *config, Source *sources, Visibility *visibilities);
void saveVisibilities(Config *config, Visibility *visibilities);
double randomInRange(double min, double max);
double sampleNormal();

#endif /* DIRECT_FOURIER_TRANSFORM_H_ */

#ifdef __cplusplus
}
#endif

