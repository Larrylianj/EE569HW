# EE569 Homework Assignment #2# Date:		Feb 9, 2014# Name: 	Siyang Liang# ID: 		3495-7475-06# E-mail: 	siyangli@usc.edu# # Compiled on OS X 10.9.1with gccprob 1(a)
	# Sobel filter	g++ -o Sobel Sobel.cpp	./Sobel [input_parameters]
	
	g++ -o EdgeMap EdgeMap.cpp
	./EdgeMap [input_parameters]	
	# LoG filter	g++ -o LoG LoG.cpp	./LoG [input_parameters]
	
	(b)
	g++ -o RGB_background RGB_background.cpp	./RGB_background [input_parameters]
	
	# Need to include “colorspace.h”
	gcc -o CIE_background CIE_background.cpp	./CIE_background [input_parameters]
	
	g++ -o Contour Contour.cpp	./Contour [input_parameters]	prob 2(a)
	# Pre-Processing	g++ -o ContrastEnhancement ContrastEnhancement.cpp	./ContrastEnhancement [input_parameters]		g++ -o RGB2GRAY RGB2GRAY.cpp	./RGB2GRAY [input_parameters]	
	# Post-Processing	g++ -o Fingerprint_Enhancement Fingerprint_Enhancement.cpp	./Fingerprint_Enhancement [input_parameters]		(b)
	# Default is shrink	g++ -o shrinkThin shrinkThin.cpp	./shrinkThin [input_parameters]
	
	g++ -o RidgeCounting RidgeCounting.cpp	./RidgeCounting [input_parameters]

	# For display Histogram (Matlab)	PlayHist.m

	(c)
	g++ -o MinutiaeDetect MinutiaeDetect.cpp	./MinutiaeDetect [input_parameters]prob 3(a)	# Need to specify the method used	g++ -o dither dither.cpp	./dither [input_parameters]	g++ -o Floyd_Steinberg_Error_diffusion Floyd_Steinberg_Error_diffusion.cpp	./Floyd_Steinberg_Error_diffusion [input_parameters]		(b)	g++ -o Floyd_Steinberg_Error_diffusion Floyd_Steinberg_Error_diffusion.cpp	./Floyd_Steinberg_Error_diffusion [input_parameters]

	g++ -o MBVQ_ErrorDiffusion MBVQ_ErrorDiffusion.cpp	./MBVQ_ErrorDiffusion [input_parameters]