
// Copyright 2019 Adam Campbell, Seth Hall, Andrew Ensor
// High Performance Computing Research Laboratory, 
// Auckland University of Technology (AUT)
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "direct_fourier_transform.h"

// Initializes the configuration of the algorithm
void init_config(Config *config)
{
	// Number of sources to synthesize if no file provided.
	config->num_sources = 1;

	// Use fixed sources (not from file).
	config->synthetic_sources = false;

	// Use fixed visibilities (not from file).
	config->synthetic_visibilities = true;

	// If using synthetic visibility creation, set this flag
	// to Gaussian distribute random visibility positions.
	config->gaussian_distribution_sources = false;

	// Force synthetic visibilities to have a 0 w term,
	// or generate a random w term between min_w and max_w
	config->forceZeroWTerm = false;

	// Cache File for Sources
	// File format : first row = number of sources in file
	// subsequent rows = each unique source in the form:
	// l, m, intensity
	// note: data can be either single or double precision
	config->source_file = "../example_sources.txt";

	// Cache File for Visibilities
	// File format : first row = number of visibilities in file
	// subsequent rows = each unique visibility in the form:
	// u, v, w, brightness (real), brightness (imag), intensity
	// note: data can be either single or double precision
	config->vis_file    = "../example_visibilities.txt";

	// Dimension of Fourier domain grid
	config->grid_size = 1024;

	// Fourier domain grid cell size in radians
	config->cell_size = 4.848136811095360e-06;

	// Frequency of visibility uvw terms
	config->frequency_hz = 300e6;

	// Scalar for visibility coordinates
	config->uv_scale = config->grid_size * config->cell_size;

	// Range for visibility u coordinates for synthesizing data
	config->min_u = -(config->grid_size / 2.0);
	config->max_u = config->grid_size / 2.0;

	// Range for visibility v coordinates for synthesizing data
	config->min_v = -(config->grid_size / 2.0);
	config->max_v = config->grid_size / 2.0;

	// Range for visibility w coordinates for synthesizing data
	config->min_w = config->min_v; // fixed to v coordinates for now
	config->max_w = config->max_v; // fixed to v coordinates for now

	// Number of visibilities per source to synthesize
	// if no file provided.
	config->num_visibilities = 5;

	// Seed random from time (used for synthetic data)
	srand(time(NULL));
}

// Loads sources into memory from some source file, or generates
// synthetic sources for testing purposes.
void load_sources(Config *config, Source **sources)
{
	if(config->synthetic_sources)
	{
		printf(">>> UPDATE: Using synthetic Sources...\n\n");
		*sources = calloc(config->num_sources, sizeof(Source));

		if(*sources == NULL) 
		{
			printf(">>> ERROR: Unable to allocate memory for sources...\n\n");
			return;
		}

		// synthesize n sources
		for(int src_indx = 0; src_indx < config->num_sources; ++src_indx)
		{
			(*sources)[src_indx] = (Source) {
                .l = random_in_range(config->min_u,config->max_u) * config->cell_size,
                .m = random_in_range(config->min_v,config->max_v) * config->cell_size,
                .intensity = 1.0}; // fixed intensity for testing purposes
		}
	}
	else // Using sources from file
	{
		printf(">>> UPDATE: Using Sources from file...\n\n");
		FILE *file = fopen(config->source_file, "r");
        if(file == NULL)
    	{
    		printf(">>> ERROR: Unable to locate sources file...\n\n");
    		return;
    	}

        // Reading in the counter for number of sources
		fscanf(file, "%d\n", &(config->num_sources));
		*sources = calloc(config->num_sources, sizeof(Source));

		if(*sources == NULL) 
        {
        	printf(">>> ERROR: Unable to allocate memory for sources...\n\n");
            if(file) fclose(file);
            return;
        }

		double l = 0.0;
		double m = 0.0;
		double intensity = 0.0;

		// Read in n number of sources
		for(int src_indx = 0; src_indx < config->num_sources; ++src_indx)
		{
			// Read in provided source attributes 
			fscanf(file, "%lf %lf %lf\n", &l, &m, &intensity);

			(*sources)[src_indx] = (Source) {
                .l = l * config->cell_size,
                .m = m * config->cell_size,
                .intensity = intensity};
		}

		// Clean up
		fclose(file);
		printf(">>> UPDATE: Successfully loaded %d sources from file..\n\n", config->num_sources);
	}
}

// Loads visibilities into memory from some source file, or generates
// synthetic visibilities for testing purposes. Gaussian distribution
// is optional.
void load_visibilities(Config *config, Visibility **visibilities)
{
	// Using synthetic visibilities
	if(config->synthetic_visibilities)
	{
		printf(">>> UPDATE: Using synthetic Visibilities...\n\n");
		*visibilities =  calloc(config->num_visibilities, sizeof(Visibility));
		
		if(*visibilities == NULL)
		{
			printf(">>> ERROR: Unable to allocate memory for visibilities...\n\n");
		 	return;
		}

		double gaussian_u = 1.0;
		double gaussian_v = 1.0;
		double gaussian_w = 1.0;

		//try randomize visibilities in the center of the grid
		for(int vis_indx = 0; vis_indx < config->num_visibilities; ++vis_indx)
		{	
			// Using gaussian distribution
            if(config->gaussian_distribution_sources)
			{
            	gaussian_u = generate_sample_normal();
				gaussian_v = generate_sample_normal();
				gaussian_w = generate_sample_normal();
			}

			// Generating the random u,v coordinates of this visibility
			double u = random_in_range(config->min_u,config->max_u) * gaussian_u;
			double v = random_in_range(config->min_v,config->max_v) * gaussian_v;
			double w = (config->forceZeroWTerm) ? 0.0
				: random_in_range(config->min_w / 10.0, config->max_w / 10.0) * gaussian_w;

			(*visibilities)[vis_indx] = (Visibility) {
                .u = u / config->uv_scale,
                .v = v / config->uv_scale,
            	.w = w / config->uv_scale};
		}
	}
	else // Using visibilities from file
	{
		printf(">>> UPDATE: Using Visibilities from file...\n\n");
		FILE *file = fopen(config->vis_file, "r");
        if(file == NULL)
    	{
    		printf(">>> ERROR: Unable to locate visibilities file...\n\n");
    		return;
    	}

        // Reading in the counter for number of visibilities
		fscanf(file, "%d\n", &(config->num_visibilities));
		*visibilities = calloc(config->num_visibilities, sizeof(Visibility));

		// File found, but was memory allocated?
		if(*visibilities == NULL) 
        {
        	printf(">>> ERROR: Unable to allocate memory for visibilities...\n\n");
            if(file) fclose(file);
            return;
        }

		double u = 0.0;
		double v = 0.0;
		double w = 0.0;
		Complex brightness = (Complex) {.real = 0.0, .imaginary = 0.0};
		double intensity = 0.0;

		// Used to scale visibility coordinates from wavelengths
		// to meters
		double wavelength_to_meters = config->frequency_hz / C;

		// Read in n number of visibilities
		for(int vis_indx = 0; vis_indx < config->num_visibilities; ++vis_indx)
		{
			// Read in provided visibility attributes
			// u, v, w, brightness (real), brightness (imag), intensity
			fscanf(file, "%lf %lf %lf %lf %lf %lf\n", &u, &v, &w, &(brightness.real),
				 &(brightness.imaginary), &intensity);

			(*visibilities)[vis_indx] = (Visibility) {
                .u = u * wavelength_to_meters,
                .v = v * wavelength_to_meters,
                .w = (config->forceZeroWTerm) ? 0.0 : w * wavelength_to_meters,
            	.brightness.real = brightness.real,
            	.brightness.imaginary = brightness.imaginary,
            	.intensity = 1.0}; // fixed to 1.0 (for now)
		}

		// Clean up
		fclose(file);
		printf(">>> UPDATE: Successfully loaded %d visibilities from file..\n\n",config->num_visibilities);
	}
}

// Performs the inverse direct fourier transformation to obtain the complex brightness
// of each visibility from each identified source. This is the meat of the algorithm.
void extract_visibilities(Config *config, Source *sources, Visibility *visibilities)
{
	printf(">>> UPDATE: Performing extraction of visibilities from sources...\n\n");

	for(int vis_idx = 0; vis_idx < config->num_visibilities; ++vis_idx)
	{
		Visibility *vis = &visibilities[vis_idx];
		Complex source_sum = (Complex) {.real = 0.0, .imaginary = 0.0};

		// For all sources, obtain some portion of brightness
		for(int src_indx = 0; src_indx < config->num_sources; ++src_indx)
		{
			Source *src             = &sources[src_indx];
			double image_correction = sqrt(1.0 - pow(src->l, 2.0) - pow(src->m, 2.0));
			double theta            = vis->u * src->l + vis->v * src->m + vis->w * (image_correction - 1.0);
			
			Complex theta_complex = (Complex) {
                			.real =  cos(2.0 * M_PI * theta),
					   .imaginary = -sin(2.0 * M_PI * theta)
			};

			double normalized_intensity     = src->intensity / image_correction;
			theta_complex.real             *= normalized_intensity;
			theta_complex.imaginary        *= normalized_intensity;
			source_sum.real                += theta_complex.real;
			source_sum.imaginary           += theta_complex.imaginary;
		}

		vis->brightness = source_sum;
	}

	printf(">>> UPDATE: Visibility extraction complete...\n\n");
}

// Saves the extracted visibility data to file
// note: file format is first row is the number of visibilities
// every subsequent row represents a unique visibility in the
// form (u, v, w, brightness (real), brightness (imag), intensity)
void save_visibilities(Config *config, Visibility *visibilities)
{
	// Save visibilities to file
	FILE *file = fopen(config->vis_file, "w");

	// Unable to open file
	if(file == NULL)
	{
		printf(">>> ERROR: Unable to save visibilities to file...\n\n");
		return;
	}

	printf(">>> UPDATE: Writing visibilities to file...\n\n");

	// Record number of visibilities
	fprintf(file, "%d\n", config->num_visibilities);

	// Used to scale visibility coordinates from meters to
	// wavelengths (useful for gridding, inverse DFT etc.)
	double meters_to_wavelengths = config->frequency_hz / C;

	// Record individual visibilities
	for(int vis_indx = 0; vis_indx < config->num_visibilities; ++vis_indx)
	{
		// u, v, w, real, imag, intensity
		fprintf(file, "%lf %lf %lf %lf %lf %lf\n",
            visibilities[vis_indx].u / meters_to_wavelengths,
			visibilities[vis_indx].v / meters_to_wavelengths,
		    visibilities[vis_indx].w / meters_to_wavelengths,
            visibilities[vis_indx].brightness.real,
		    visibilities[vis_indx].brightness.imaginary,
		    1.0); // static intensity (for now)
	}

	// Clean up
	fclose(file);
	printf(">>> UPDATE: Completed writing of visibilities to file...\n\n");
}

// Generate a random double in range [min, max)
double random_in_range(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

// Generate a gaussian distribution multiplier
// note: used to better "sample" synthetic visibilities
// u, v coordinates instead of randomly distributed
double generate_sample_normal()
{
	double u = ((double) rand() / RAND_MAX) * 2.0 - 1.0;
	double v = ((double) rand() / RAND_MAX) * 2.0 - 1.0;
	double r = u * u + v * v;
	if(r <= 0.0 || r > 1.0)
		return generate_sample_normal();
	return u * sqrt(-2.0 * log(r) / r);
}
