
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "direct_fourier_transform.h"

void init_config(Config *config)
{
	/// Number of sources to process
	config->num_sources = 1;

	// Use fixed sources (not from file)
	config->synthetic_sources = false;

	// Use fixed visibilities (not from file)
	config->synthetic_visibilities = true;

	// if using synthetic visibility creation, set this flag to Gaussian distribute random visibility positions
	config->gaussian_distribution_sources = false;

	// Origin of Sources
	config->source_file = "../sources.txt";

	// Destination of Visibilities
	config->vis_file    = "../visibilities.txt";

	// Dimension of Fourier domain grid
	config->grid_size = 1024.0;

	// Fourier domain grid cell size in radians
	config->cell_size = 4.848136811095360e-06;

	// Frequency of visibility uvw terms
	config->frequency_hz = 300000000.0;

	// Scalar for visibility coordinates
	config->uv_scale = config->grid_size * config->cell_size;

	// Range for visibility u coordinates
	config->min_u = -(config->grid_size / 2.0);
	config->max_u = config->grid_size / 2.0;

	// Range for visibility v coordinates
	config->min_v = -(config->grid_size / 2.0);
	config->max_v = config->grid_size / 2.0;

	// Number of visibilities per source
	config->num_visibilities = 100;

	// Seed random from time
	srand(time(NULL));
}

void load_sources(Config *config, Source **sources)
{
	if(config->synthetic_sources)
	{
		printf(">>> UPDATE: Using synthetic Sources...\n\n");

		*sources = calloc(config->num_sources, sizeof(Source));
		if(*sources == NULL) return;

		for(int n = 0; n < config->num_sources; ++n)
		{
            double half_grid = config->grid_size/2.0;
			(*sources)[n] = (Source) {
                .l = random_in_range(-half_grid, half_grid) * config->cell_size,
                .m = random_in_range(-half_grid, half_grid) * config->cell_size,
                .intensity = 1.0};
		}
	}
	else
	{
		printf(">>> UPDATE: Using Sources from file...\n\n");
		FILE *file = fopen(config->source_file, "r");
        if(!file) return;

		fscanf(file, "%d\n", &(config->num_sources));
		*sources = calloc(config->num_sources, sizeof(Source));
		if(*sources == NULL) 
        {
            if(file) fclose(file);
            return;
        }

		PRECISION l, m, intensity;

		for(int i=0; i<config->num_sources; ++i)
		{
			fscanf(file,"%f %f %f\n",&l ,&m, &intensity);
			(*sources)[i] = (Source) {
                .l = l * config->cell_size,
                .m = m * config->cell_size,
                .intensity = intensity};
		}

		printf(">>> UPDATE: Successfully loaded %d sources from file..\n\n",config->num_sources);
		fclose(file);
	}
}

void load_visibilities(Config *config, Visibility **visibilities)
{
	if(config->synthetic_visibilities)
	{
		printf(">>> UPDATE: Using synthetic Visibilities...\n\n");
		*visibilities =  calloc(config->num_visibilities, sizeof(Visibility));
		if(*visibilities == NULL)  return;

		double gaussian_u = 1.0;
		double gaussian_v = 1.0;
        double u = 0.0;
        double v = 0.0;

		//try randomize visibilities in the center of the grid
		for(int i=0;i<config->num_visibilities;++i)
		{	
            if(config->gaussian_distribution_sources)
			{
            	gaussian_u = generate_sample_normal();
				gaussian_v = generate_sample_normal();
			}

			u = random_in_range(config->min_u,config->max_u) * gaussian_u;
			v = random_in_range(config->min_v,config->max_v) * gaussian_v;
			(*visibilities)[i] = (Visibility) {
                .u = u / config->uv_scale,
                .v = v / config->uv_scale};
		}
	}
	else
	{
		printf(">>> UPDATE: Using Visibilities from file...\n\n");

		// Load from file
	}
}

void perform_extraction(Config *config, Source *sources, Visibility *visibilities)
{
	printf(">>> UPDATE: Performing extraction of visibilities from sources...\n\n");

	// For all visibilities
	for(int v = 0; v < config->num_visibilities; ++v)
	{
		Visibility *vis = &visibilities[v];
		Complex sourceSum = (Complex) {.real = 0.0, .imaginary = 0.0};

		// For all sources
		for(int s = 0; s < config->num_sources; ++s)
		{
			Source *src = &sources[s];
			double srcSqrt = sqrt(1.0 - pow(src->l, 2.0) - pow(src->m, 2.0));
			double theta = vis->u*src->l + vis->v*src->m + vis->w*(srcSqrt-1.0);
			Complex thetaComplex = (Complex) {
                .real = cos(2.0 * M_PI * theta),
				.imaginary = -sin(2.0 * M_PI * theta)};
			thetaComplex.real *= src->intensity / srcSqrt;
			thetaComplex.imaginary *= src->intensity / srcSqrt;
			sourceSum.real += thetaComplex.real;
			sourceSum.imaginary += thetaComplex.imaginary;
		}

		vis->intensity = sourceSum;
	}

	printf(">>> UPDATE: Extraction complete...\n\n");
}

void save_visibilities(Config *config, Visibility *visibilities)
{
	// Save visibilities to file
	FILE *file = fopen(config->vis_file, "w");

	// Unable to open file
	if(!file)
	{
		printf(">>> ERROR: Unable to save visibilities to file...\n\n");
		return;
	}

	printf(">>> UPDATE: Writing visibilities to file...\n\n");

	// Record number of visibilities
	fprintf(file, "%d\n", config->num_visibilities);

	// Scalar from meters to wavelengths
	double wavelength_to_meters = config->frequency_hz / C;

	// Record individual visibilities
	for(int n = 0; n < config->num_visibilities; ++n)
	{
		// u, v, w, real, imag, intensity
		fprintf(file, "%f %f %f %f %f %f\n", 
            visibilities[n].u / wavelength_to_meters,
			visibilities[n].v / wavelength_to_meters,
		    visibilities[n].w / wavelength_to_meters,
            visibilities[n].intensity.real,
		    visibilities[n].intensity.imaginary,
		    1.0);
	}

	// Clean up
	fclose(file);

	printf(">>> UPDATE: Completed writing of visibilities to file...\n\n");
}

double random_in_range(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double generate_sample_normal()
{
	double u = ((double) rand()/RAND_MAX) * 2.0 - 1.0;
	double v = ((double) rand()/RAND_MAX) * 2.0 - 1.0;
	double r = u*u + v*v;
	if(r <= 0.0 || r > 1.0)
		return generate_sample_normal();
	return u * sqrt(-2.0 * log(r)/r);
}


