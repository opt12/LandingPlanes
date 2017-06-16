/*
 ============================================================================
 Name        : readInTiff.c
 Author      : Felix Eckstein; Matr-# 8161569
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
/* includes big portions from tifffastcrop

 v. 1.3.8

 Copyright (c) 2013-2016 Christophe Deroulers

 Portions are based on libtiff's tiffcp code. tiffcp is:
 Copyright (c) 1988-1997 Sam Leffler
 Copyright (c) 1991-1997 Silicon Graphics, Inc.

 Distributed under the GNU General Public License v3 -- contact the
 author for commercial use */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy
#include <tiff.h>
#include <tiffio.h>
#include <assert.h>

#include "config.h"	//TODO: Das ist unschön, wenn wir nicht auch autoconfig und autoheaders benutzen wollen.

#include "readInTiff.h"

/* Return 0 if the requested memory size exceeds the machine's
 addressing size type (size_t) capacity or if biptspersample is
 unhandled */
static size_t computeMemorySize(uint16 spp, uint16 bitspersample,
		uint32 outwidth, uint32 outlength) {
	uint16 bitsperpixel = spp * bitspersample;
	uint64 memorysize = outlength;
	if (bitsperpixel % 8 == 0)
		memorysize *= (uint64) outwidth * (bitsperpixel / 8);
	else if (8 % bitsperpixel == 0) {
		int pixelsperbyte = 8 / bitsperpixel;
		int bytesperrow = (outwidth + pixelsperbyte - 1) / pixelsperbyte;
		memorysize *= bytesperrow;
	} else
		return 0;

	if ((size_t) memorysize != memorysize)
		return 0;
	return memorysize;
}

static void cpBufToBuf(uint8* out_beginningofline, uint32 out_x,
	uint8* in_beginningofline, uint32 in_x,
	uint32 widthtocopyinpixels, uint16 bitsperpixel,
	uint32 rows, int out_linewidthinbytes, int in_linewidthinbytes)
{
	if (bitsperpixel % 8 == 0) { /* Easy case */
		uint8* in  = in_beginningofline  + in_x  * (bitsperpixel/8);
		uint8* out = out_beginningofline + out_x * (bitsperpixel/8);
		while (rows-- > 0) {
			memcpy(out, in, widthtocopyinpixels * (bitsperpixel/8));
			in += in_linewidthinbytes;
			out += out_linewidthinbytes;
		}
		return;
	}

	/* Hard case. Do computations to prepare steps 1, 2, 3: */
	static const uint8 left_masks[] =
	    { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

	assert(8 % bitsperpixel == 0);
	int pixelsperbyte = 8/bitsperpixel;
	int out_x_start_whole_outbytes = out_x;
	int in_x_start_whole_outbytes  = in_x;
	uint32 in_skew  = in_linewidthinbytes;
	uint32 out_skew = out_linewidthinbytes;

	 /* 1. Copy pixels to complete the first byte (if incomplete) of dest. */
	int out_startpositioninincompletefirstbyte = out_x % pixelsperbyte;
	int in_startpositioninfirstbyte = in_x % pixelsperbyte;
	int out_pixelsinincompletefirstbyte = 0;
	int shift_first_inbyte_to_first_incompl_outbyte =
		out_startpositioninincompletefirstbyte -
		in_startpositioninfirstbyte;
	int shift_second_inbyte_to_first_incompl_outbyte = 0;
	uint8 mask_of_first_inbyte_to_first_incompl_outbyte = 0;
	uint8 mask_of_second_inbyte_to_first_incompl_outbyte = 0;

	if (out_startpositioninincompletefirstbyte) {
		out_pixelsinincompletefirstbyte =
		    8-out_startpositioninincompletefirstbyte;
		if (out_pixelsinincompletefirstbyte > widthtocopyinpixels)
			out_pixelsinincompletefirstbyte = widthtocopyinpixels;
		int pixels_available_in_first_inbyte =
		    8 - in_startpositioninfirstbyte;

		if (pixels_available_in_first_inbyte >= out_pixelsinincompletefirstbyte) {
			mask_of_first_inbyte_to_first_incompl_outbyte =
			    left_masks[out_pixelsinincompletefirstbyte * bitsperpixel] >>
				(in_startpositioninfirstbyte * bitsperpixel);
		} else {
			mask_of_first_inbyte_to_first_incompl_outbyte =
			    left_masks[pixels_available_in_first_inbyte *
					bitsperpixel] >>
				(in_startpositioninfirstbyte * bitsperpixel);
			mask_of_second_inbyte_to_first_incompl_outbyte =
			    left_masks[(out_pixelsinincompletefirstbyte -
				pixels_available_in_first_inbyte) * bitsperpixel];
			shift_second_inbyte_to_first_incompl_outbyte =
			    pixels_available_in_first_inbyte;
			in_skew--;
		}

		in_x_start_whole_outbytes += out_pixelsinincompletefirstbyte;
		out_x_start_whole_outbytes += out_pixelsinincompletefirstbyte;
	}

	/* 2. Write as many whole bytes as possible in dest. */
	/* Examples of bits in in_bytes:
	  6+2|8|1+7 -> (6+)2|6+2|6(+2)  11 bits -> 2 bytes, 1 whole byte
	  6+2|8|7+1 -> (6+)2|6+2|6+2    17 bits -> 3 bytes, 2 whole bytes
	   Strategy: copy whole bytes. Then make an additional,
	  incomplete byte (which shall have
	  out_pixelsinincompletelastbyte pixels) with the remaining (not
	  yet copied) bits of current byte in input and, if these bits
	  are not enough, the first in_bitstoreadinlastbyte bits of next
	  byte in input. */
	uint8* out_wholeoutbytes = out_beginningofline +
		out_x_start_whole_outbytes / pixelsperbyte;
	uint8* in = in_beginningofline + in_x / pixelsperbyte;
	uint32 wholebytesperline =
	    ((widthtocopyinpixels-out_pixelsinincompletefirstbyte) *
		bitsperpixel) / 8;
	int in_bitoffset = (in_x_start_whole_outbytes % pixelsperbyte) * bitsperpixel;
	if (in_bitoffset) {
		in_skew -= wholebytesperline + 1;
		out_skew -= wholebytesperline;
	}

	/* 3. Copy pixels to complete the last byte (if incomplete) of dest. */
	int out_pixelsinincompletelastbyte =
		(out_x + widthtocopyinpixels) % pixelsperbyte;
	if (out_pixelsinincompletelastbyte) {
		if (in_bitoffset == 0)
			wholebytesperline++; /* Let memcpy start writing last byte */
		else
			out_skew--;
	}
	int in_bitstoreadinlastbyte =
	    out_pixelsinincompletelastbyte * bitsperpixel - (8-in_bitoffset);
	if (in_bitstoreadinlastbyte < 0)
		in_bitstoreadinlastbyte = 0;

	/* Perform steps 1, 2, 3: */
	while (rows-- > 0) {
		/* 1. */
		if (out_startpositioninincompletefirstbyte) {
			uint8 a = (*in) & mask_of_first_inbyte_to_first_incompl_outbyte;
			if (shift_first_inbyte_to_first_incompl_outbyte >= 0)
				a >>= shift_first_inbyte_to_first_incompl_outbyte;
			else
				a <<= -shift_first_inbyte_to_first_incompl_outbyte;
			if (shift_second_inbyte_to_first_incompl_outbyte) {
				in++;
				a |= ((*in) & mask_of_second_inbyte_to_first_incompl_outbyte)
				    >> shift_second_inbyte_to_first_incompl_outbyte;
			}

			*(out_wholeoutbytes-1) &= left_masks[out_startpositioninincompletefirstbyte];
			*(out_wholeoutbytes-1) |= a;
		}

		/* 2. & 3. */
		if (in_bitoffset == 0) {
			memcpy(out_wholeoutbytes, in, wholebytesperline);
			/* out_pixelsinincompletelastbyte =
			 in_bitstoreadinlastbyte / bitsperpixel in this case */
			if (in_bitstoreadinlastbyte)
				*(out_wholeoutbytes + wholebytesperline) |=
				    (*(in+wholebytesperline))
				    >> (8-in_bitstoreadinlastbyte);
		} else {
			uint32 j = wholebytesperline;
			uint8 acc = (*in++) << in_bitoffset;

			while (j-- > 0) {
				acc |= (*in) >> (8-in_bitoffset);
				*out_wholeoutbytes++ = acc;
				acc = (*in++) << in_bitoffset;
			}
			if (out_pixelsinincompletelastbyte) {
				if (in_bitstoreadinlastbyte)
					acc |= (*in) >> (8-in_bitstoreadinlastbyte);
				*out_wholeoutbytes++ = acc;
			}
		}

		in += in_skew;
		out_wholeoutbytes += out_skew;
	}
}

static int cpTiles2Strip(TIFF* in, uint32 xmin, uint32 ymin,
	uint32 width, uint32 length, unsigned char * outbuf,
	tsize_t outscanlinesizeinbytes, uint16 bitsperpixel)
{
	tmsize_t inbufsize;
	uint32 intilewidth = (uint32) -1, intilelength = (uint32) -1;
	tsize_t intilewidthinbytes = TIFFTileRowSize(in);
	uint32 y;
	unsigned char * inbuf, * bufp= outbuf;
	int error = 0;

	TIFFGetField(in, TIFFTAG_TILEWIDTH, &intilewidth);
	TIFFGetField(in, TIFFTAG_TILELENGTH, &intilelength);

	inbufsize= TIFFTileSize(in);
	inbuf = (unsigned char *)_TIFFmalloc(inbufsize); /* not malloc
	    because TIFFTileSize returns a tmsize_t */
	if (!inbuf) {
		TIFFError(TIFFFileName(in),
				"Error, can't allocate space for image buffer");
		return (EXIT_INSUFFICIENT_MEMORY);
	}

	for (y = ymin ; y < ymin + length + intilelength ; y += intilelength) {
		uint32 x, out_x = 0;
		uint32 yminoftile = (y/intilelength) * intilelength;
		uint32 ymintocopy = ymin > yminoftile ? ymin : yminoftile;
		uint32 ymaxplusone = yminoftile + intilelength;
		uint32 lengthtocopy;
		unsigned char * inbufrow = inbuf +
		    intilewidthinbytes * (ymintocopy-yminoftile);

		if (ymaxplusone > ymin + length)
			ymaxplusone = ymin + length;
		if (ymaxplusone <= yminoftile)
			break;
		lengthtocopy = ymaxplusone - ymintocopy;

		for (x = xmin ; x < xmin + width + intilewidth ;
		    x += intilewidth) {
			uint32 xminoftile = (x/intilewidth) * intilewidth;
			uint32 xmintocopyintile = xmin > xminoftile ?
			    xmin : xminoftile;
			uint32 xmaxplusone = xminoftile + intilewidth;

			if (xmaxplusone > xmin + width)
				xmaxplusone = xmin + width;
			if (xmaxplusone <= xminoftile)
				break;

			uint32 widthtocopyinpixels =
			    xmaxplusone - xmintocopyintile;

			if (TIFFReadTile(in, inbuf, xminoftile,
			    yminoftile, 0, 0) < 0) {
				TIFFError(TIFFFileName(in),
				    "Error, can't read tile at "
				    UINT32_FORMAT ", " UINT32_FORMAT,
				    xminoftile, yminoftile);
				error = EXIT_IO_ERROR;
				goto done;
			}

			cpBufToBuf(bufp, out_x, inbufrow,
			    xmintocopyintile-xminoftile,
			    widthtocopyinpixels, bitsperpixel,
			    lengthtocopy, outscanlinesizeinbytes,
			    intilewidthinbytes);
			out_x += widthtocopyinpixels;
		}
		bufp += outscanlinesizeinbytes * lengthtocopy;
	}

	done:
	_TIFFfree(inbuf);
	return error;
}

static int cpStrips2Strip(TIFF* in,
	uint32 xmin, uint32 ymin, uint32 width, uint32 length,
	unsigned char * outbuf, tsize_t outscanlinesizeinbytes,
	uint16 bitsperpixel,
	uint32 * y_of_last_read_scanline, uint32 inimagelength)
{
	tsize_t inbufsize;
	uint16 input_compression;
	tsize_t inwidthinbytes = TIFFScanlineSize(in);
	uint32 y;
	unsigned char * inbuf, * bufp= outbuf;
	int error = 0;

	TIFFGetFieldDefaulted(in, TIFFTAG_COMPRESSION, &input_compression);
	inbufsize= TIFFScanlineSize(in);  /* not malloc because
	    TIFFScanlineSize returns a tmsize_t */
	inbuf = (unsigned char *)_TIFFmalloc(inbufsize);
	if (!inbuf) {
		TIFFError(TIFFFileName(in),
				"Error, can't allocate space for image buffer");
		return (EXIT_INSUFFICIENT_MEMORY);
	}

	/* When compression method doesn't support random access: */
	if (input_compression == COMPRESSION_JPEG) {
		uint32 y;

		if (*y_of_last_read_scanline > ymin) {
		/* If we need to go back, finish reading to the end,
		 * then a restart will be automatic */
			for (y = *y_of_last_read_scanline + 1 ;
			     y < inimagelength ; y++)
				if (TIFFReadScanline(in, inbuf, y, 0) < 0) {
					TIFFError(TIFFFileName(in),
					    "Error, can't read scanline at "
					    UINT32_FORMAT " for exhausting",
					    y);
					error = EXIT_IO_ERROR;
					goto done;
				} else
					*y_of_last_read_scanline= y;
		}

		/* Then read up to the point we want to start
		 * copying at */
		for (y = 0 ; y < ymin ; y++)
			if (TIFFReadScanline(in, inbuf, y, 0) < 0) {
				TIFFError(TIFFFileName(in),
				    "Error, can't read scanline at "
				    UINT32_FORMAT " for exhausting", y);
				error = EXIT_IO_ERROR;
				goto done;
			} else
				*y_of_last_read_scanline= y;
	}

	for (y = ymin ; y < ymin + length ; y++) {
		uint32 xmintocopyinscanline = xmin;

		if (TIFFReadScanline(in, inbuf, y, 0) < 0) {
			TIFFError(TIFFFileName(in),
			    "Error, can't read scanline at "
			    UINT32_FORMAT " for copying", y);
			error = EXIT_IO_ERROR;
			goto done;
		} else
			*y_of_last_read_scanline= y;

		cpBufToBuf(bufp, 0, inbuf, xmintocopyinscanline,
		    width, bitsperpixel, 1,
		    outscanlinesizeinbytes, inwidthinbytes);
		bufp += outscanlinesizeinbytes;
	}

	done:
	_TIFFfree(inbuf);
	return error;
}

int getImageInformation(tileCharacteristics* info, const char * infilename) {
	TIFF * in;

	in = TIFFOpen(infilename, "r");
	if (in == NULL) {
		return EXIT_IO_ERROR;
	}


	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &(info->outwidth));
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &(info->outlength));
	TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &(info->spp));
	TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &(info->bitspersample));

	info->memsize = 0;

	TIFFClose(in);
	return EXIT_SUCCESS;
}


int makeExtractFromTIFFFile(const extractionParameters p,
		tileCharacteristics* tile, const char * infilename) {
	TIFF * in;
	uint32 inimagewidth, inimagelength;
	tile->outwidth = p.requestedwidth;
	tile->outlength = p.requestedlength;
	uint16 planarconfig;
//	void * out; /* TIFF* or FILE* */
	uint32 y_of_last_read_scanline = 0;
	int return_code = 0; /* Success */

	if (tile->outwidth == 0 || tile->outlength == 0) {
		fprintf(stderr, "Requested extract is empty. Can't do it. Aborting.\n");
		return EXIT_GEOMETRY_ERROR;
	}

	in = TIFFOpen(infilename, "r");
	if (in == NULL) {
		if (p.verbose)
			fprintf(stderr, "Unable to open file \"%s\".\n", infilename);
		return EXIT_IO_ERROR;
	}

	if (p.verbose)
		fprintf(stderr, "File \"%s\" open.\n", infilename);

	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &inimagewidth);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &inimagelength);
	TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &(tile->spp));
	TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &(tile->bitspersample));

	if (tile->bitspersample % 8 != 0 && 8 % tile->bitspersample != 0) {
		TIFFError(TIFFFileName(in), "Error, can't deal with file with "
				"bits-per-sample %d (not a multiple nor a "
				"divisor of 8)", tile->bitspersample);
		TIFFClose(in);
		return EXIT_UNHANDLED_FILE_TYPE;
	}

	if (p.verbose)
		fprintf(stderr,
				"File \"%s\" has %u bits per sample and %u samples per pixel.\n",
				infilename, (unsigned) tile->bitspersample,
				(unsigned) tile->spp);

	if (p.requestedxmin > inimagewidth || p.requestedymin > inimagelength) {
		fprintf(stderr,
				"Requested top left corner is outside the image. Aborting.\n");
		TIFFClose(in);
		return EXIT_GEOMETRY_ERROR;
	}
	if (p.requestedxmin + tile->outwidth > inimagewidth
			|| p.requestedymin + tile->outlength > inimagelength) {
		if (p.requestedxmin + tile->outwidth > inimagewidth)
			tile->outwidth = inimagewidth - p.requestedxmin;
		if (p.requestedymin + tile->outlength > inimagelength)
			tile->outlength = inimagelength - p.requestedymin;
		if (p.verbose)
			fprintf(stderr, "Requested rectangle extends "
					"outside the image. Adjusting "
					"dimensions to " UINT32_FORMAT "x"
			UINT32_FORMAT ".\n", tile->outwidth, tile->outlength);
	}

	TIFFGetField(in, TIFFTAG_PLANARCONFIG, &planarconfig);
	if (planarconfig != PLANARCONFIG_CONTIG) {
		TIFFError(TIFFFileName(in), "Error, can't deal with file with "
				"planar configuration %d (non contiguous)", planarconfig);
		TIFFClose(in);
		return EXIT_UNHANDLED_FILE_TYPE;
	}

	tile->memsize = computeMemorySize(tile->spp, tile->bitspersample,
			tile->outwidth, tile->outlength);
	tile->buf = tile->memsize == 0 ? NULL : malloc(tile->memsize);
	if (tile->buf == NULL) {
		fprintf(stderr, "Unable to allocate enough memory to prepare extract ("
		UINT64_FORMAT " bytes needed).\n", tile->memsize);
		TIFFClose(in);
		return EXIT_INSUFFICIENT_MEMORY;
	}

	{
		uint16 bitsperpixel = tile->bitspersample * tile->spp;
		tsize_t outscanlinesizeinbytes;
		int error = 0;

//		tiffCopyFieldsButDimensions(in, out);
//		TIFFSetField(out, TIFFTAG_IMAGEWIDTH, tile->outwidth);
//		TIFFSetField(out, TIFFTAG_IMAGELENGTH, tile->outlength);
//		TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, tile->outlength);
//
//		testAndFixOutTIFFPhotoAndCompressionParameters(in, out);
//		/* To be done *after* setting compression --
//		 * otherwise, ScanlineSize may be wrong */
//		outscanlinesizeinbytes = TIFFScanlineSize(out);
		outscanlinesizeinbytes = tile->outwidth*bitsperpixel/8;	//TODO: check ob das korrekt gerechnet ist.

		if (((TIFFIsTiled(in)
				&& !(error = cpTiles2Strip(in, p.requestedxmin, p.requestedymin,
						tile->outwidth, tile->outlength, (unsigned char *)tile->buf,	//this typecast is necessary as we expect floats in our buffer
						outscanlinesizeinbytes, bitsperpixel)))
				|| (!TIFFIsTiled(in)
						&& !(error = cpStrips2Strip(in, p.requestedxmin,
								p.requestedymin, tile->outwidth,
								tile->outlength, (unsigned char *)tile->buf,
								outscanlinesizeinbytes, bitsperpixel,
								&y_of_last_read_scanline, inimagelength))))) {
			if (p.verbose)
				fprintf(stderr, "Extract prepared.\n");
		} else
			return_code = error;
	}

	if (return_code == 0 && p.verbose)
		fprintf(stderr, "Extract prepared.\n");
	TIFFClose(in);
	return return_code;
}
