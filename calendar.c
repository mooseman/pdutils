/*********************************************************************/
/*                                                                   */
/*  This Program Written by Paul Edwards.                            */
/*                                                                   */
/*********************************************************************/
/*********************************************************************/
/*                                                                   */
/*  Calendar - produce a calendar for any given year in the          */
/*  proleptic Gregorian calendar.                                    */
/*                                                                   */
/*  This program takes as a command line parameter a single number   */
/*  which should be a 4 digit year.  It then prints out a calendar   */
/*  corresponding to that year.  This program was inspired by a      */
/*  shareware program which performed a similar function but didn't  */
/*  come with source!                                                */
/*                                                                   */
/*  This program is released to the public domain.                   */
/*                                                                   */
/*  Written 1990-08-28.                                              */
/*                                                                   */
/*  Modified 1991-01-29 to go from Monday to Sunday instead of       */
/*  Sunday to Saturday, in accordance to ISO's recommendations.      */
/*                                                                   */
/*  Modified 1992-11-18 to make use of a different day-of-week       */
/*  routine which didn't suffer from the limitations of the last     */
/*  (the last one required the year to be from 1901-2099).  The new  */
/*  one's limitations are from 400 BC to 32367 AD as far as I can    */
/*  tell.                                                            */
/*                                                                   */
/*  Modified 1993-01-28 to make use of a new day-of-week macro       */
/*  because the last one was bad as well.  The new one has a limit   */
/*  of 0001-01-01 to 14699-12-31                                     */
/*                                                                   */
/*  Modified 1993-01-29 to fix up the new day-of-week macro          */
/*  I intend replacing the macro again sometime                      */
/*                                                                   */
/*  Modified 1993-01-31 to use my new day-of-week macro which has    */
/*  undergone extensive stress-testing and been shown to be valid    */
/*  over the range 0001-01-01 to 32767-01-01.  It will produce       */
/*  unknown results for dates outside this range, although it should */
/*  still return values from 0..6 so function correctly.             */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

static int dayno(int yyyy, int mm, int x, int y);
static void prt3mon(int yr, int a, char *prtmon);

main(int argc, char **argv)
{
  int yr, wrkyr, dig1, dig2, dig3, dig4;

  if (argc < 2)
  {
    printf("Calendar - print a calendar\n");
    printf("Written by Paul Edwards, Sydney, Australia\n");
    printf("You must enter 1 parameter - the 4-digit\n");
    printf("year you want printed\n");
    printf("This program then prints a calendar for that year\n");
    return (EXIT_FAILURE);
  }
  yr = atoi(*(argv+1));
  wrkyr = yr;
  dig1 = wrkyr/1000;  wrkyr%=1000;
  dig2 = wrkyr/100;  wrkyr%=100;
  dig3 = wrkyr/10;  wrkyr%=10;
  dig4 = wrkyr;
  printf("                                %d %d %d %d\n\n\n\n\n",
      dig1,dig2,dig3,dig4);
  prt3mon(yr, 0, "       JANUARY                 FEBRUARY"
      "                   MARCH");
  prt3mon(yr, 1, "        APRIL                     MAY"
      "                     JUNE");
  prt3mon(yr, 2, "        JULY                    AUGUST"
      "                  SEPTEMBER");
  prt3mon(yr, 3, "       OCTOBER                 NOVEMBER"
      "                 DECEMBER");
  return (0);
}

static void prt3mon(int yr, int a, char *prtmon)
{
  static char *letters = " m  t  w  t  f  s  s     "
                         " m  t  w  t  f  s  s     "
                         " m  t  w  t  f  s  s\n";
  int b, i, j, x;

  printf("%s\n\n",prtmon);
  printf("%s",letters);
  for (i=0;i<6;i++)
  {
    for (b=1;b<=3;b++)
    {
      for (j=0;j<7;j++)
      {
        x = dayno(yr,a*3+b,i,j);
        if (x) printf("%2d ",x);
        else printf("   ");
      }
      printf("    ");
    }
    printf("\n");
  }
  printf("\n\n\n\n");
  return;
}

#define isleap(year) ((((year%4)==0) && ((year%100)!=0)) || \
    ((year%400)==0))

#define dow(y,m,d) \
  ((((((m)+9)%12+1) * 26 - 2)/10 + (d) + \
  ((y)%400+400) + ((y)%400+400)/4 - ((y)%400+400)/100 + \
  (((m) <= 2) ? ( \
  (((((y)%4)==0) && (((y)%100)!=0)) || (((y)%400)==0)) \
  ? 5 : 6) : 0)) % 7)

static int dayno(int yyyy, int mm, int x, int y)
{
  static int daytab[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31};
  int a, b, c;

  a = dow(yyyy,mm,1);
  b = x * 7 + y;
  c = daytab[mm-1];
  if ((mm == 2) && isleap(yyyy)) c++;
  if (b < a) return (0);
  if ((b-a) >= c) return (0);
  return ((b-a+1));
}

