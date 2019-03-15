
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "config.h"

int main(int argc, const char* argv[])
{
	printf(">>> AUT HPC Research Laboratory - Direct Fourier Transform <<<\n");
	Config config;
	initConfig(&config);

	int numberOfSources = 1;
	Source *sources = calloc(numberOfSources, sizeof(Source));
	int numberOfVisibilities = 1;
	Visibility *visibilities = calloc(numberOfVisibilities, sizeof(Visibility));

	// Obtain Sources from file
	loadSources(sources, config.source_file);

	// Perform extraction of visibilities
	if(config.gpu_enabled)
	{
		printf(">>> Performing visibility extraction using GPU\n");
		// Call CUDA kernel here
	}
	else
	{
		printf(">>> Performing visibility extraction using CPU\n");
		performExtraction(sources, visibilities);
	}

	// Save visibilities to file

	// Clean up
	free(visibilities);
	free(sources);

	return EXIT_SUCCESS;
}

void initConfig (Config *config)
{
	// Use parallelization
	config->gpu_enabled = true;
	// Origin of Sources
	config->source_file = "input_file.txt";
	// Destination of Visibilities
	config->vis_file    = "output_file.txt";
}

void loadSources(Source *sources, char* source_file)
{
	// load from somewhere... file?
}

void performExtraction(Source *sources, Visibility *visibilities)
{
	// Perform the extraction of visibilities from sources
}
