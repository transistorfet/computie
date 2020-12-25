 
#ifndef _PROTOTYPE_H
#define _PROTOTYPE_H

#ifdef BUILD_KERNEL
	#define MAIN(name)	name
#else
	#define MAIN(name)	main
#endif

#endif

