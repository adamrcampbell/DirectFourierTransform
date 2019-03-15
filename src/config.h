
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
	PRECISION weight;
} Visibility;

typedef struct Config {
	char *source_file;
	char *vis_file;
	bool gpu_enabled;
} Config;


//=========================//
//     Function Headers    //
//=========================//
void initConfig (Config *config);
void loadSources(Source *sources, char* source_file);
void performExtraction(Source *sources, Visibility *visibilities);

#endif /* CONFIG_H_ */
