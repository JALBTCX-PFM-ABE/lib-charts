
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

#include "charts.h"

/***************************************************************************\
*                                                                           *
*   Module Name:        swap_int32_t                                        *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       July 1992                                           *
*                                                                           *
*   Module Security                                                         *
*   Classification:     Unclassified                                        *
*                                                                           *
*   Data Security                                                           *
*   Classification:     Unknown                                             *
*                                                                           *
*   Purpose:            This function swaps bytes in a four byte int.       *
*                                                                           *
*   Arguments:          word                -   pointer to the int32_t      *
*                                                                           *
\***************************************************************************/

void charts_swap_int32_t (int32_t *word)
{
    uint32_t    temp[4];

    temp[0] = *word & 0x000000ff;

    temp[1] = (*word & 0x0000ff00) >> 8;

    temp[2] = (*word & 0x00ff0000) >> 16;

    temp[3] = (*word & 0xff000000) >> 24;

    *word = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + temp[3];
}



/***************************************************************************\
*                                                                           *
*   Module Name:        swap_float                                          *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       July 1992                                           *
*                                                                           *
*   Module Security                                                         *
*   Classification:     Unclassified                                        *
*                                                                           *
*   Data Security                                                           *
*   Classification:     Unknown                                             *
*                                                                           *
*   Purpose:            This function swaps bytes in a four byte float.     *
*                                                                           *
*   Arguments:          word                -   pointer to the float        *
*                                                                           *
\***************************************************************************/

void charts_swap_float (float *word)
{
    uint32_t    temp[4];

    union
    {
        uint32_t    iword;
        float    fword;
    } eq;

    eq.fword = *word;

    temp[0] = eq.iword & 0x000000ff;

    temp[1] = (eq.iword & 0x0000ff00) >> 8;

    temp[2] = (eq.iword & 0x00ff0000) >> 16;

    temp[3] = (eq.iword & 0xff000000) >> 24;

    eq.iword = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + temp[3];

    *word = eq.fword;

    return;
}



/***************************************************************************\
*                                                                           *
*   Module Name:        swap_double                                         *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       January 2000                                        *
*                                                                           *
*   Module Security                                                         *
*   Classification:     Unclassified                                        *
*                                                                           *
*   Data Security                                                           *
*   Classification:     Unknown                                             *
*                                                                           *
*   Purpose:            This function swaps bytes in an eight byte double.  *
*                                                                           *
*   Arguments:          word                -   pointer to the double       *
*                                                                           *
\***************************************************************************/

void charts_swap_double (double *word)
{
    int32_t    i;
    char     temp;
    union
    {
        char     bytes[8];
        double  d;
    }eq;
    
    memcpy (&eq.bytes, word, 8);

    for (i = 0 ; i < 4 ; i++)
    {
        temp = eq.bytes[i];
        eq.bytes[i] = eq.bytes[7 - i];
        eq.bytes[7 - i] = temp;
    }

    *word = eq.d;
}

void charts_swap_int64_t (int64_t *word)
{
    union
    {
      double d;
      uint64_t i;
    } eq;

    eq.i = *word;
    charts_swap_double (&eq.d);

    *word = eq.i;
}


/***************************************************************************\
*                                                                           *
*   Module Name:        swap_int16_t                                        *
*                                                                           *
*   Programmer(s):      Jan C. Depner                                       *
*                                                                           *
*   Date Written:       October 1996                                        *
*                                                                           *
*   Module Security                                                         *
*   Classification:     Unclassified                                        *
*                                                                           *
*   Data Security                                                           *
*   Classification:     Unknown                                             *
*                                                                           *
*   Purpose:            This function swaps bytes in a two byte int.        *
*                                                                           *
*   Arguments:          word                -   pointer to the int16_t      *
*                                                                           *
\***************************************************************************/

void charts_swap_int16_t (int16_t *word)
{
    int16_t   temp;

    swab (word, &temp, 2);

    *word = temp;

    return;
}
