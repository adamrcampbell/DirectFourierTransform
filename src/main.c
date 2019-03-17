
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "config.h"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

int main(int argc, const char* argv[])
{
	printf("==============================================================\n");
	printf(">>> AUT HPC Research Laboratory - Direct Fourier Transform <<<\n");
	printf("==============================================================\n\n");
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
	config->numSources = 1;

	// Use fixed sources (not from file)
	config->synthetic_sources = true;

	// Use fixed visibilities (not from file)
	config->synthetic_visibilities = true;

	// Use parallelization
	config->gpu_enabled = false;

	// Origin of Sources
	config->source_file = "input_file.txt";

	// Destination of Visibilities
	config->vis_file    = "output_file.txt";

	// Range for visibility u coordinates
	config->min_u = -5000.0;
	config->max_u = 5000.0;

	// Range for visibility v coordinates
	config->min_v = -5000.0;
	config->max_v = 5000.0;

	// Visibility distribution across image
	config->distanceBetweenVis = 100.0;
	config->numVisAcrossU = (int) fabs(config->min_u - config->max_u) / config->distanceBetweenVis;
	config->numVisAcrossV = (int) fabs(config->min_v - config->max_v) / config->distanceBetweenVis;

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
			sources[n] = (Source) {.l = 0.0,
								   .m = 0.0,
								   .intensity = 1.0};
		}
	}
	else
	{
		printf(">>> UPDATE: Using Sources from file...\n\n");
		// Load from file
	}
}

void loadVisibilities(Config *config, Visibility *visibilities)
{
	if(config->synthetic_visibilities)
	{
		printf(">>> UPDATE: Using synthetic Visibilities...\n\n");

		for(int n = 0; n < config->numVisibilities; ++n)
		{
			// Distribute visibilities over various u and v coordinates
		}
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
		Complex sourceSum = (Complex) {.real = 0.0, .imaginary = 0.0};

		// For all sources
		for(int n = 0; n < config->numSources; ++n)
		{
			Source *s = &sources[n];
			double x = sqrt(1.0 - pow(s->l, 2.0) - pow(s->m, 2.0));
			// double theta =
		}


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

Complex complexMult(Complex z1, Complex z2)
{
	return (Complex) {.real      = z1.real * z2.real - z1.imaginary * z2.imaginary,
					  .imaginary = z1.real * z2.imaginary + z1.imaginary * z2.real};
}
