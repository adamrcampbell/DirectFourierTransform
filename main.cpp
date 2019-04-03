
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

	// Obtain Sources from file, or synthesize
	Source *sources = NULL;
	load_sources(&config, &sources);
    // Unable to obtain Sources
	if(sources == NULL)
	{	
        printf(">>> ERROR: Source memory was unable to be allocated \n");
		return EXIT_FAILURE;
	}

	// Obtain Visibilities from file, or synthesize
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
