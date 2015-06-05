/*
** File   :	cfe_pep_memport.c
**
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. 
**
** Author :	Ezra Yeheskeli
**
** Purpose:
**		   This file  contains some of the cFE Platform Support Layer.
**         It contains the processor architecture specific calls.
**
**  16-Nov-2003 Ezra Yeheskeli
**          - First Creation.
**
*/

/*
** Include section
*/

#include "cfe_psp.h"


/*
** global memory
*/




/*
** Name: CFE_PSP_PortRead8
**
** Purpose:
**         Read one byte of memory.
**
**
** Parameters:
**	PortAddress : Address to be read
**  ByteValue  : The address content will be copied to the location pointed by
**            this argument
**
** Global Inputs: None
**
** Global Outputs: None
**
**
**
** Return Values: CFE_PSP_SUCCESS
*/
int32 CFE_PSP_PortRead8( uint32 PortAddress, uint8 *ByteValue )
{

	(*ByteValue) = (uint8)*((uint8 *)PortAddress) ;

	return(CFE_PSP_SUCCESS) ;
}

/*
** Name: CFE_PSP_PortWrite8
**
** Purpose:
**         Write one byte of memory.
**
**
** Parameters:
**	PortAddress : Address to be written to
**  ByteValue  : The content pointed by this argument will be copied to the
**            address
**
** Global Inputs: None
**
** Global Outputs: None
**
**
**
** Return Values:
**		CFE_PSP_SUCCESS
*/
int32 CFE_PSP_PortWrite8 ( uint32 PortAddress, uint8 ByteValue )
{
   *((uint8 *)PortAddress) = ByteValue;
	return(CFE_PSP_SUCCESS) ;

}

/*
** Name: CFE_PSP_PortRead16
**
** Purpose:
**         Read  2 bytes of memory.
**
**
** Parameters:
**	PortAddress : Address to be read
**  uint16Value : The address content will be copied to the location pointed by
**            this argument
**
** Global Inputs: None
**
** Global Outputs: None
**
**
**
** Return Values:
**		CFE_PSP_SUCCESS
**		CFE_PSP_ERROR_ADD_MISALIGNED The Address is not aligned to 16 bit
**      addressing scheme.
*/
int32 CFE_PSP_PortRead16( uint32 PortAddress, uint16 *uint16Value )
{
	/* check 16 bit alignment  , check the 1st lsb */
	if( PortAddress & 0x00000001)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}
	(*uint16Value) = (uint16)*((uint16 *)PortAddress) ;
	return(CFE_PSP_SUCCESS) ;

}


/*
** Name: CFE_PSP_PortWrite16
**
** Purpose:
**         Write 2 byte of memory.
**
**
** Parameters:
**	PortAddress : Address to be written to
**  uint16Value : The content pointed by this argument will be copied to the
**            address
**
** Global Inputs: None
**
** Global Outputs: None
**
**
**
** Return Values:
**      CFE_PSP_SUCCESS
**		CFE_PSP_ERROR_ADD_MISALIGNED The Address is not aligned to 16 bit
**      addressing scheme.
*/
int32 CFE_PSP_PortWrite16 ( uint32 PortAddress, uint16 uint16Value )
{
	/* check 16 bit alignment  , check the 1st lsb */
	if( PortAddress & 0x00000001)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}
   *((uint16 *)PortAddress) = uint16Value;
	return(CFE_PSP_SUCCESS) ;
}

/*
** Name: CFE_PSP_PortRead32
**
** Purpose:
**         Read 4 bytes of memory.
**
**
** Parameters:
**	PortAddress : Address to be read
**  uint32Value : The address content will be copied to the location pointed by
**            this argument
**
** Global Inputs: None
**
** Global Outputs: None
**
**
**
** Return Values:
**		CFE_PSP_SUCCESS
**		CFE_PSP_ERROR_ADD_MISALIGNED The Address is not aligned to 16 bit
**      addressing scheme.
*/
int32 CFE_PSP_PortRead32( uint32 PortAddress, uint32 *uint32Value )
{
	/* check 32 bit alignment  */
	if( PortAddress & 0x00000003)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}
	(*uint32Value) = *((uint32 *)PortAddress) ;
	return(CFE_PSP_SUCCESS) ;

}

/*
** Name: CFE_PSP_PortWrite32
**
** Purpose:
**         Write 4 byte of memory.
**
**
** Parameters:
**	PortAddress : Address to be written to
**  uint32Value : The content pointed by this argument will be copied to the
**            address
**
** Global Inputs: None
**
** Global Outputs: None
**
**
**
** Return Values:
**		CFE_PSP_SUCCESS
**		CFE_PSP_ERROR_ADD_MISALIGNED The Address is not aligned to 16 bit
**      addressing scheme.
*/
int32 CFE_PSP_PortWrite32 ( uint32 PortAddress, uint32 uint32Value )
{
	/* check 32 bit alignment  */
	if( PortAddress & 0x00000003)
	{
		return(CFE_PSP_ERROR_ADDRESS_MISALIGNED) ;
	}
   *((uint32 *)PortAddress) = uint32Value;
	return(CFE_PSP_SUCCESS) ;

}

