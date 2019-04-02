
// Entry point to C based Direct Fourier Transform

#include <cstdlib>
#include <cstdio>

#include "direct_fourier_transform.h"

int main(int argc, char **argv)
{
    printf("==============================================================\n");
	printf(">>> AUT HPC Research Laboratory - Direct Fourier Transform <<<\n");
	printf("==============================================================\n\n");

    // Initialise the DFT configuration
	Config config;
	init_config(&config);

	// Obtain Sources from file
	Source *sources = NULL;
	load_sources(&config, &sources);
    // Unable to obtain Sources
	if(sources == NULL)
	{	
        printf(">>> ERROR: Source memory was unable to be allocated \n");
		return EXIT_FAILURE;
	}

	// Obtain Visibilities from file
	Visibility *visibilities = NULL;
	load_visibilities(&config, &visibilities);

    // Unable to obtain visibilities
	if(visibilities == NULL)
	{	printf(">>> ERROR: Visibility memory was unable to be allocated \n");
	    if(sources) free(sources);
		return EXIT_FAILURE;
	}

	// Perform extraction of visibilities
	printf(">>> UPDATE: Performing visibility extraction using CPU...\n\n");
	perform_extraction(&config, sources, visibilities);

	// Save visibilities to file
	save_visibilities(&config, visibilities);

	// Clean up
	if(visibilities) free(visibilities);
	if(sources)      free(sources);

	printf(">>> UPDATE: Direct Fourier Transform operations complete, exiting...\n\n");
    
	return EXIT_SUCCESS;
}
