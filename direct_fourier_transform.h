
#ifdef __cplusplus
extern "C" {
#endif

#ifndef DIRECT_FOURIER_TRANSFORM_H_
#define DIRECT_FOURIER_TRANSFORM_H_

//=========================//
//   Algorithm Constants   //
//=========================//

// Pi (double precision)
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

// Speed of light
#ifndef C
	#define C 299792458.0
#endif

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
	int num_visibilities;
	int num_sources;
	char *source_file;
	char *vis_file;
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
void init_config (Config *config);
void load_sources(Config *config, Source **sources);
void load_visibilities(Config *config, Visibility **visibilities);
void perform_extraction(Config *config, Source *sources, Visibility *visibilities);
void save_visibilities(Config *config, Visibility *visibilities);
double random_in_range(double min, double max);
double generate_sample_normal(void);

#endif /* DIRECT_FOURIER_TRANSFORM_H_ */

#ifdef __cplusplus
}
#endif

