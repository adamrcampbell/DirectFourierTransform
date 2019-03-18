
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "config.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
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

	Source *sources = calloc(config.numSources, sizeof(Source));
	Visibility *visibilities = calloc(config.numVisibilities, sizeof(Visibility));

	// Obtain Sources from file
	loadSources(&config, sources);
	// Obtain Visibilities from file
	loadVisibilities(&config, visibilities);

	// Perform extraction of visibilities
	if(config.gpu_enabled)
	{
		printf(">>> UPDATE: Performing visibility extraction using GPU...\n\n");
		// Call CUDA kernel here
	}
	else
	{
		printf(">>> UPDATE: Performing visibility extraction using CPU...\n\n");
		performExtraction(&config, sources, visibilities);
	}

	// Save visibilities to file
	saveVisibilities(&config, visibilities);

	// Clean up
	if(visibilities) free(visibilities);
	if(sources)      free(sources);

	printf(">>> UPDATE: Direct Fourier Transform operations complete, exiting...\n\n");

	return EXIT_SUCCESS;
}

void initConfig (Config *config)
{
	/// Number of sources to process
	config->numSources = 100;

	// Use fixed sources (not from file)
	config->synthetic_sources = true;

	// Use fixed visibilities (not from file)
	config->synthetic_visibilities = true;

	// Use parallelization
	config->gpu_enabled = false;

	// Origin of Sources
	config->source_file = "DFT_sources.txt";

	// Destination of Visibilities
	config->vis_file    = "DFT_visibilities.txt";

	// Dimension of Fourier domain grid (TESTING ONLY)
	config->grid_size = 18000.0;

	// Fourier domain grid cell size in radians (TESTING ONLY)
	config->cell_size = 0.00000639708380288949;

	// Scalar for visibility coordinates
	config->uv_scale = config->grid_size * config->cell_size;

	// Range for visibility u coordinates
	config->min_u = -500.0;
	config->max_u = 500.0;

	// Range for visibility v coordinates
	config->min_v = -500.0;
	config->max_v = 500.0;

	// Visibility distribution across image
	config->distanceBetweenVis = 2.5;
	config->numVisAcrossU = (int) (fabs(config->min_u - config->max_u) / config->distanceBetweenVis) + 1.0;
	config->numVisAcrossV = (int) (fabs(config->min_v - config->max_v) / config->distanceBetweenVis) + 1.0;

	// Number of visibilities per source
	config->numVisibilities = config->numVisAcrossU * config->numVisAcrossV;
}

void loadSources(Config *config, Source *sources)
{
	if(config->synthetic_sources)
	{
		printf(">>> UPDATE: Using synthetic Sources...\n\n");

		for(int n = 0; n < config->numSources; ++n)
		{
			sources[n] = (Source) {.l = 0.0, //randomInRange(0.0, 0.5),
								   .m = 0.0, //randomInRange(0.0, 0.5),
								   .intensity = 0.001};
		}
	}
	else
	{
		printf(">>> UPDATE: Using Sources from file...\n\n");

//		FILE *file = fopen(config.source_file, "r");
//
//		// Unable to open file
//		if(!file)
//		{
//			printf(">>> ERROR: Unable to load sources from file...\n\n");
//			return;
//		}
//
//		fclose(file);
	}
}

void loadVisibilities(Config *config, Visibility *visibilities)
{
	if(config->synthetic_visibilities)
	{
		printf(">>> UPDATE: Using synthetic Visibilities...\n\n");
		int visCounter = 0;

		// Distribute equally over min and max v
		for(double v = config->min_v; v <= config->max_v; v += config->distanceBetweenVis)
		{
			// Distribute equally over min and max u
			for(double u = config->min_u; u <= config->max_u; u += config->distanceBetweenVis)
			{
				visibilities[visCounter++] = (Visibility) {.u = u, .v = v};
				// printf("U: %f, V: %f\n", u, v);
			}
		}

		printf("Total Vis: %d, config total vis: %d\n", visCounter, config->numVisibilities);
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
		// vis->u *= config->uv_scale;
		// vis->v *= config->uv_scale;
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
	// Record individual visibilities
	for(int n = 0; n < config->numVisibilities; ++n)
	{
		// u, v, w, real, imag
		fprintf(file, "%f %f %f %f %f\n", visibilities[n].u,
										   visibilities[n].v,
										   visibilities[n].w,
										   visibilities[n].intensity.real,
										   visibilities[n].intensity.imaginary);
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
