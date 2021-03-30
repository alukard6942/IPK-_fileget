/**
 * File: log.h
 * Author: alukard <alukard@github>
 * Date: 25.03.2021
 */


#ifdef LOG
	#define LOG(arg) do {std::cout<< __FILE__ << ":" << __LINE__ << " " << #arg << " => " << arg << std::endl;} while (0);
#else 
	#define LOG(arg) ;;
#endif

