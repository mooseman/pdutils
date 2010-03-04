/*********************************************************************/
/*                                                                   */
/*  This Program Written By Paul Edwards.                            */
/*  Released to the public domain.                                   */
/*                                                                   */
/*********************************************************************/
/*********************************************************************/
/*                                                                   */
/*  wc - count words                                                 */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <ctype.h>

int main(void)
{
    int inword = 0;
    int wordcnt = 0;
    int c;
    
    while ((c = getchar()) != EOF)
    {
        if (isspace(c))
        {
            if (inword)
            {
                inword = 0;
                wordcnt++;
            }
        }
        else
        {
            if (!inword)
            {
                inword = 1;
            }
        }
    }
    if (inword)
    {
        wordcnt++;
    }
    printf("%d\n", wordcnt);
    return (0);
}
