
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "direct_fourier_transform.h"

// Pi (double precision)
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

// Speed of light
#ifndef C
	#define C 299792458.0
#endif

int main(int argc, const char* argv[])
{
	printf("==============================================================\n");
	printf(">>> AUT HPC Research Laboratory - Direct Fourier Transform <<<\n");
	printf("==============================================================\n\n");

	// Seed random from time
	srand(time(NULL));

	Config config;
	initConfig(&config);

	// Obtain Sources from file
	Source *sources = NULL;
	loadSources(&config, &sources);
	if(sources == NULL)
	{	
        printf(">>> ERROR: Source memory was unable to be allocated \n");
		return EXIT_FAILURE;
	}

	// Obtain Visibilities from file
	Visibility *visibilities = NULL;
	loadVisibilities(&config, &visibilities);

    // Unable to obtain visibilities
	if(visibilities == NULL)
	{	printf(">>> ERROR: Visibility memory was unable to be allocated \n");
	    if(sources) free(sources);
		return EXIT_FAILURE;
	}

	// Perform extraction of visibilities
	printf(">>> UPDATE: Performing visibility extraction using CPU...\n\n");
	performExtraction(&config, sources, visibilities);

	// Save visibilities to file
	saveVisibilities(&config, visibilities);

	// Clean up
	if(visibilities) free(visibilities);
	if(sources)      free(sources);

	printf(">>> UPDATE: Direct Fourier Transform operations complete, exiting...\n\n");

	return EXIT_SUCCESS;
}

void initConfig(Config *config)
{
	/// Number of sources to process
	config->numSources = 1;

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
	config->numVisibilities = 100;
}

void loadSources(Config *config, Source **sources)
{
	if(config->synthetic_sources)
	{
		printf(">>> UPDATE: Using synthetic Sources...\n\n");
		*sources = calloc(config->numSources, sizeof(Source));
		if(*sources == NULL) return;
		for(int n = 0; n < config->numSources; ++n)
		{
			(*sources)[n] = (Source) {.l = 5.0 * config->cell_size, //randomInRange(0.0, 0.5),
								   .m = 2.0 * config->cell_size, //randomInRange(0.0, 0.5),
								   .intensity = 1.0};
		}
	}
	else
	{
		printf(">>> UPDATE: Using Sources from file...\n\n");

		FILE *file = fopen(config->source_file, "r");

		// Unable to open file
		if(!file)
		{	printf(">>> ERROR: Unable to load sources from file...\n\n");
			return;
		}

		fscanf(file, "%d\n", &(config->numSources));
		*sources = calloc(config->numSources, sizeof(Source));
		if(*sources == NULL) return;
		PRECISION l,m,intensity;
		for(int i=0;i<config->numSources;++i)
		{
			fscanf(file,"%f %f %f\n",&l,&m,&intensity);
			(*sources)[i] = (Source) {.l = l * config->cell_size,
									  .m = m * config->cell_size,
									  .intensity = intensity};
		}
		printf(">>> UPDATE: Successfully loaded %d sources from file..\n\n",config->numSources);
		fclose(file);
	}
}

void loadVisibilities(Config *config, Visibility **visibilities)
{
	if(config->synthetic_visibilities)
	{
		printf(">>> UPDATE: Using synthetic Visibilities...\n\n");
		//int visCounter = 10;

		*visibilities =  calloc(config->numVisibilities, sizeof(Visibility));
		if(*visibilities == NULL)  return;
		double gV = 1.0;
		double gU = 1.0;
		//try randomize visibilities in the center of the grid
		for(int i=0;i<config->numVisibilities;++i)
		{	if(config->gaussian_distribution_sources)
			{	gV = sampleNormal();
				gU = sampleNormal();
			}
			double u = randomInRange(config->min_u,config->max_u) * gU;
			double v = randomInRange(config->min_v,config->max_v) * gV;
			(*visibilities)[i] = (Visibility) {.u = u / config->uv_scale, .v = v / config->uv_scale};
		}
		printf("Total vis: %d\n", config->numVisibilities);
	}
	else
	{
		printf(">>> UPDATE: Using Visibilities from file...\n\n");

		// Load from file
	}
}

void performExtraction(Config *config, Source *sources, Visibility *visibilities)
{
	printf(">>> UPDATE: Performing extraction of visibilities from sources...\n\n");

	// For all visibilities
	for(int v = 0; v < config->numVisibilities; ++v)
	{
		Visibility *vis = &visibilities[v];
		//vis->u /= config->uv_scale;
		//vis->v /= config->uv_scale;
		// vis->w *= config->uv_scale;
		Complex sourceSum = (Complex) {.real = 0.0, .imaginary = 0.0};

		// For all sources
		for(int s = 0; s < config->numSources; ++s)
		{
			Source *src = &sources[s];
			double srcSqrt = sqrt(1.0 - pow(src->l, 2.0) - pow(src->m, 2.0));
			double theta = vis->u*src->l + vis->v*src->m + vis->w*(srcSqrt-1.0);
			Complex thetaComplex = (Complex) {.real = cos(2.0 * M_PI * theta),
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

void saveVisibilities(Config *config, Visibility *visibilities)
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
	fprintf(file, "%d\n", config->numVisibilities);

	// Scalar from meters to wavelengths
	double wavelengthScalar = config->frequency_hz / C;

	// Record individual visibilities
	for(int n = 0; n < config->numVisibilities; ++n)
	{
		// u, v, w, real, imag
		fprintf(file, "%f %f %f %f %f %f\n", visibilities[n].u / wavelengthScalar,
										   visibilities[n].v / wavelengthScalar,
										   visibilities[n].w / wavelengthScalar,
										   visibilities[n].intensity.real,
										   visibilities[n].intensity.imaginary,
										   1.0);
	}

	// Clean up
	fclose(file);

	printf(">>> UPDATE: Completed writing of visibilities to file...\n\n");
}

double randomInRange(double min, double max)
{
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double sampleNormal()
{
	double u = ((double) rand()/RAND_MAX) * 2 -1;
	double v = ((double) rand()/RAND_MAX) * 2 -1;
	double r = u*u + v*v;
	if(r <= 0 || r > 1)
		return sampleNormal();
	double c = sqrt(-2 * log(r)/r);
	if((u * v * c) > 1.0 || (u * v * c) < -1.0)
		return sampleNormal();
	return u * v * c;
}


