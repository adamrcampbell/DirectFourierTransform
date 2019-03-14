
#ifndef CONFIG_H_
#define CONFIG_H_

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

#endif /* CONFIG_H_ */
