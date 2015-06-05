The FS Services unit test results are expected to have the
following results for each of the FS services source files:

==========================================================================

gcov: 'cfe_fs_api.c' 100.00% coverage

==========================================================================

gcov: 'cfe_fs_priv.c' 100.00% coverage

==========================================================================

cfe_fs_decompress.c - 88.64% coverage

cfe_fs_decompress contains the code from a public domain copy of the gzip
utility that has been sightly modified to use the cFE variable types and
return codes. Other than that, the decompress algorithm is very old and
obtuse code. Most of what was not covered was because of macros that are
defined and use in the implementation. There are also lots of one letter
variables with no documentation.
==========================================================================


int32 FS_gz_eat_header( void )


        1:  233:		if ( (flags & CONTINUATION) != 0 ) {
    #####:  234:			NEXTBYTE();
    #####:  235:			if( gGuzError != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
    #####:  236:			NEXTBYTE();
    #####:  237:			if( gGuzError != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
        -:  238:		}
        -:  239:
        1:  240:		if ( (flags & EXTRA_FIELD) != 0 ) {
        -:  241:			uint32 len;
    #####:  242:			len  = NEXTBYTE();
    #####:  243:			if( gGuzError != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
    #####:  244:			len |= NEXTBYTE() << 8;
    #####:  245:			if( gGuzError != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
    #####:  246:			while (len--) {
    #####:  247:				NEXTBYTE();
    #####:  248:				if( gGuzError != CFE_SUCCESS ) return CFE_FS_GZIP_READ_ERROR_HEADER;
        -:  249:			}
        -:  250:		}


        -:  268:		/*  Discard file comment if any  */
        1:  269:		if ( (flags & COMMENT) != 0 ) {
    #####:  270:			keep_going = TRUE;
    #####:  271:			while ( keep_going ) {
        -:  272:
        -:  273:				/* Get the next byte */
    #####:  274:				thisByte = NEXTBYTE();
        -:  275:
        -:  276:				/* Header failure when end of file is reached or a read failure occurs */
    #####:  277:				if ((thisByte == EOF) || (gGuzError != CFE_SUCCESS)) return CFE_FS_GZIP_READ_ERROR_HEADER;
        -:  278:
        -:  279:				/* End of string was found */
    #####:  280:				if (thisByte == 0) keep_going = FALSE;
        -:  281:			}


int32 FS_gz_huft_build( uint32 * b, uint32 n, uint32 s, uint16 * d, uint16 * e, int32 * m )


       20:  417:   if (c[0] == n)
        -:  418:   {			/* null input--all zero length codes */
    #####:  419:		*m = 0;
    #####:  420:		return CFE_SUCCESS;
        -:  421:	}

     2197:  548:			if( (q == (HufTable *)NULL) && (j < z) )
        -:  549:			{
    #####:  550:				return CFE_FS_GZIP_BAD_CODE_BLOCK;
        -:  551:			}


int32 FS_gz_inflate( void )


        -:  600:	/*  Undo too much lookahead. The next read will be byte aligned so we can */
        -:  601:	/*  discard unused bits in the last meaningful byte. */
        2:  602:	while ( gz_bk >= 8 ) {
    #####:  603:		gz_bk -= 8;
    #####:  604:		gz_inptr--;
        -:  605:	}


int32 FS_gz_inflate_block( int32 * e )

        -:  649:	/*  inflate that block type */
        6:  650:	if      ( t == 0 ) { res = FS_gz_inflate_stored();  trace[0]++; }
        6:  651:	else if ( t == 1 ) { res = FS_gz_inflate_fixed();   trace[1]++; }
        6:  652:	else if ( t == 2 ) { res = FS_gz_inflate_dynamic(); trace[2]++; }
        -:  653:
    #####:  654:	else               res = CFE_FS_GZIP_BAD_CODE_BLOCK;
        -:  655:
        6:  656:	return res;


int32 FS_gz_inflate_codes( HufTable * tl, HufTable * td, int32 bl, int32 bd )

     2079:  704:				  NEEDBITS(e);
     2079:  705:              index = t->v.t + ( (uint32)b & mask_bits[e] );
     2079:  706:				  if ( index >= 0  &&  index < gz_hufts )
     2079:  707:                 t = &( hufTable[index] );
        -:  708:				  else
    #####:  709:                 return CFE_FS_GZIP_INDEX_ERROR;
     2079:  710:				  e = t->e;
     2079:  711:	        } while ( e > 16 );
        -:  712:		}
        -:  713:
    24135:  714:		DUMPBITS(t->b);
        -:  715:
    24135:  716:      if ( e == 16 )
        -:  717:      {                 /* then it's a literal */
        -:  718:
        -:  719:         /* gz_window[w++] = (uint8)(t->n); */
    14203:  720:         gz_window[w++] = (uint8)(t->v.n);
    14203:  721:         if ( w == WSIZE )
        -:  722:         {
    #####:  723:            gz_outcnt = w;
    #####:  724:            FS_gz_flush_window();
    #####:  725:            w = 0;
        -:  726:         }

      337:  752:               index = t->v.t + ( (uint32)b & mask_bits[e] );
      337:  753:               if ( index >= 0  &&  index < gz_hufts )
      337:  754:                  t = &( hufTable[index] );
        -:  755:               else
    #####:  756:                  return CFE_FS_GZIP_INDEX_ERROR;
      337:  757:               e = t->e;
      337:  758:            } while ( e > 16 );
        -:  759:         }


int32 FS_gz_inflate_stored( void )

        -:  946:      else
        -:  947:      {                 /* j == 18: 11 to 138 zero length codes */
    #####:  948:         NEEDBITS(7);
    #####:  949:         j = 11 + ( (uint32)b & 0x7f );
    #####:  950:         DUMPBITS(7);
    #####:  951:         if ( (uint32)i + j > n ) return CFE_FS_GZIP_BAD_DATA;
    #####:  952:         while (j--) ll[i++] = 0;
    #####:  953:         l = 0;
        -:  954:		}
        -:  955:	}

        1: 1069:	DUMPBITS(n);
        -: 1070:
        -: 1071:	/*  get the length and its complement */
        1: 1072:	NEEDBITS(16);
    #####: 1073:	n = ( (uint32)b & 0xffff );
    #####: 1074:	DUMPBITS(16);
        -: 1075:
    #####: 1076:	NEEDBITS(16);
    #####: 1077:	if ( n != (uint32)( (~b) & 0xffff) )  return CFE_FS_GZIP_BAD_DATA;    /* error in compressed data */
    #####: 1078:	DUMPBITS(16);
        -: 1079:
        -: 1080:
        -: 1081:	/*  read and output the compressed data */
    #####: 1082:	while (n--)
        -: 1083:   {
    #####: 1084:		NEEDBITS(8);
    #####: 1085:      gz_window[w++] = (uint8)b;
    #####: 1086:		if ( w == WSIZE )
        -: 1087:      {
    #####: 1088:	        gz_outcnt = w;
    #####: 1089:	        FS_gz_flush_window();
    #####: 1090:	        w = 0;
        -: 1091:		}
    #####: 1092:		DUMPBITS(8);
        -: 1093:	}
        -: 1094:
        -: 1095:	/*  restore the globals from the locals */
    #####: 1096:	gz_outcnt = w;					/* restore global window pointer */
    #####: 1097:	gz_bb     = b;					/* restore global bit buffer */
    #####: 1098:	gz_bk     = k;
        -: 1099:
    #####: 1100:	return CFE_SUCCESS;
        -: 1101:
        -: 1102:}

