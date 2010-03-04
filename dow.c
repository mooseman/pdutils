/* dow - compute day of week given year (e.g. 1993), month and
   day.  It returns 0..6 for Sunday..Saturday, to be compatible
   with the C standard library.  It has been tested over the range 
   0001-01-01 to 32767-01-01.  It should work from 0001-01-01
   to INT_MAX-01-01.  Negative years will not cause a problem, they
   just produce results which assume their was a year 0. */

/* Written by Paul Edwards, 1993-01-28 */
/* Released to the Public Domain */
/* You may sell this, use it in commercial programs, etc, with no
   legal concern. */

/* The rules for a leap year are that if the year is divisible by
   4 then it is a leap year, unless it is divisible by 100, then
   it isn't, unless it is divisible by 400 then it is after all.
   ie, 1900 wasn't a leap year, but 2000 will be.  Basically what
   this means is that a cycle goes for 400 years.  An additional
   fact is that for any given date, the day of the week in 400
   years time is the same.  Now every normal year, the day of
   the week advances by 1 day (because there are 52 weeks and 1
   day in the year).  Except for leap years, where there is an
   additional day advanced. */

/* This program converts the above logic into code.  It even
   checks to make sure the month and day are valid.  It is
   easy to follow this code.  However, an alternative is to
   use some brilliant tricky maths, which happens to work, just
   because it does, not for any other reason.  Basically, given
   the month, it is possible to work out how many days to
   compensate, without having to use a table.  Great stuff, and
   far more fun than this boring algorithm.  When you consider
   the fact that the algorithm doesn't need to be modified, you
   have every reason not to use this code. */

/* Some would argue that finding the day of the week is not
   likely to be a time critical task.  Some would argue that
   24 integers is not a lot of storage.  Some would argue that
   public domain code is really really great.  I personally
   reckon that fancy maths is really really great, so long as
   you don't want to understand it.  I personally intend to use
   an impossible-to-read macro to do this. */


/* see above for the leap year rule - also look up "leap year"
   in your dictionary */

#define isleap(year) \
    ((((year%4)==0) && ((year%100)!=0)) || ((year%400)==0)) 


int dow(int yr, int mo, int da)
{
  static int totdays[12] =
      {0,31,59,90,120,151,181,212,243,273,304,334};
  static int numdays[12] =
      {31,28,31,30,31,30,31,31,30,31,30,31};

  int addon=0;  /* number of days that have advanced */
  int leap;     /* is this year a leap year? */

  yr %= 400;
  if (yr < 0) yr += 400;  /* for silly buggers who pretend they
                             want to know a BC date */ 

  if (isleap(yr)) leap = 1;  /* is the current year a leap year? */
  else leap = 0;

  if ((mo < 1) || (mo > 12)) return (-1);  /* validate the month */
  if (da < 1) return (-1);                 /* and day of month */
  if (leap && (mo == 2))
  {
    if (da > (numdays[mo-1]+1)) return (-1);
  }
  else if (da > numdays[mo-1]) return (-1);

  addon += yr;            /* The day advances by one day every year */
  addon += yr/4;          /* An additional day if it is divisible by 4 */
  addon -= yr/100;        /* Unless it is divisible by 100 */
  if (leap && (mo <= 2)) addon--;  /* However, we should not count that
                             extra day if the current year is a leap
                             year and we haven't gone past 29th February */

  addon += totdays[mo-1]; /* The day of the week increases by
                             the number of days in all the months
                             up till now */

  addon += da;      /* the day of week advances for each day */

  /* Now as we all know, 2000-01-01 is a Saturday.  Using this
  as our reference point, and the knowledge that we want to
  return 0..6 for Sunday..Saturday, we find out that we need to
  compensate by adding 6. */

  addon += 6;

  return (addon%7);  /* the remainder after dividing by 7
                        gives the day of week */
}
