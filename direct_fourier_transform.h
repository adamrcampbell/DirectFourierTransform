
// Copyright 2019 AUT High Performance Computing Research Laboratory
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

// Dynamic square root subsitution
#ifndef SQRT
	#if SINGLE_PRECISION
		#define SQRT sqrtf
	#else
		#define SQRT sqrt
	#endif
#endif

// Dynamic power subsitution
#ifndef POW
	#if SINGLE_PRECISION
		#define POW powf
	#else
		#define POW pow
	#endif
#endif

// Dynamic cosine subsitution
#ifndef COS
	#if SINGLE_PRECISION
		#define COS cosf
	#else
		#define COS cos
	#endif
#endif

// Dynamic logarithm subsitution
#ifndef LOG
	#if SINGLE_PRECISION
		#define LOG logf
	#else
		#define LOG log
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
	PRECISION min_u;
	PRECISION max_u;
	PRECISION min_v;
	PRECISION max_v;
	PRECISION grid_size;
	PRECISION cell_size;
	PRECISION uv_scale;
	PRECISION frequency_hz;
} Config;


//=========================//
//     Function Headers    //
//=========================//
void init_config (Config *config);

void load_sources(Config *config, Source **sources);

void load_visibilities(Config *config, Visibility **visibilities);

void perform_extraction(Config *config, Source *sources, Visibility *visibilities);

void save_visibilities(Config *config, Visibility *visibilities);

PRECISION random_in_range(PRECISION min, PRECISION max);

PRECISION generate_sample_normal(void);

#endif /* DIRECT_FOURIER_TRANSFORM_H_ */

#ifdef __cplusplus
}
#endif

