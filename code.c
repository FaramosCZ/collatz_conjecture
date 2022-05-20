#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define  ASCII_ZERO 48

// Declare glbal pointer to memory in which each byte will represent a char.
const unsigned short size_of_unsigned_short = sizeof(unsigned short);

unsigned short *data = NULL;
unsigned int size_of_allocated_memory;

unsigned short carry = 0;

// In each iteration two files are used
// From one the old_value is read, to the second the new_value is written
// The initial file is always named 'seed'.
// This is then changed to name 'result_x', where 'x' is the 'filename_num' variable which is incremented in each iteration
char filename[200] = "seed";
int filename_num = 1;

// ------------------------------
int load_number_from_file()
  {
   carry = 0;

   FILE *fp;
   fp = fopen(filename, "r");
   if ( fp == NULL )
     {
      fprintf(stderr, "Unable to open file");
      return 1;
     }

   fseek(fp, 0, SEEK_END);
   printf("\t\t\tNumber of digits: %ld", ftell(fp));
   size_of_allocated_memory = size_of_unsigned_short * ftell(fp) ;
   data = malloc(size_of_allocated_memory);
   if ( data == NULL )
     {
      fprintf(stderr, "Unable to allocate enough memory. Tried to allocate %d bytes", size_of_allocated_memory);
      fclose(fp);
      return 1;
     }

   fseek(fp, 0, SEEK_SET);

   for(unsigned int i = 0 ; i < (size_of_allocated_memory / size_of_unsigned_short) ; i++)
     {
      *(data+i) = getc(fp) - ASCII_ZERO;
     }

  if (fclose(fp))
    {
     printf("error closing file.");
     exit(-1);
    }

   return 0;
  }

// ------------------------------
int print_number(FILE * stream)
  {
   if(carry != 0) fprintf(stream, "%c", carry+ASCII_ZERO);

   for(unsigned int i = 0; i < (size_of_allocated_memory / size_of_unsigned_short) ; i++)
     {
      if( i==0 && *(data+i) == 0) continue;
      fprintf(stream, "%c", *(data+i) + ASCII_ZERO );
     }

   if (stream == stdout ) printf("\n");

   return 0;
  }

// ------------------------------
// The operation is actually 'new_value = 3 * old_value + 1'
// That's why carry value is set to 1 at the beginning

int multiply()
  {
   carry = 1;
   unsigned short value;

   for(unsigned int i = size_of_unsigned_short; i <= (size_of_allocated_memory / size_of_unsigned_short) ; i++)
     {
       value = *(data+(size_of_allocated_memory / size_of_unsigned_short)-i);
       value = (value * 3) + carry;

       carry = value / 10;
       value = value % 10;

       *(data+(size_of_allocated_memory / size_of_unsigned_short)-i) = value;
     }

   return 0;
  }

// ------------------------------
int save_number_to_file()
  {
   FILE *fp;

   sprintf(filename,"result_%d",filename_num);
   //filename_num++;

   fp = fopen(filename, "w");
   if ( fp == NULL )
     {
      fprintf(stderr, "Unable to open file '%s'", filename);
      return 1;
     }

   print_number(fp);

   if (fclose(fp))
     {
      printf("error closing file.");
      exit(-1);
     }

   return 0;
  }

// ------------------------------
int divide()
  {
   carry = 0;
   unsigned short value;

   for(unsigned int i = 0; i < (size_of_allocated_memory / size_of_unsigned_short) ; i++)
     {
      value = *(data+i) + carry*10;
      carry = value % 2;
      value = value / 2;

      *(data+i) = value;
     }

   carry = 0;
   return 0;
  }

// ------------------------------
int main()
  {

   unsigned long iteration = 1;

   do
     {
      printf("Iteration: %lu", iteration++);

       load_number_from_file();
       //print_number(stdout);

       if( *(data + (size_of_allocated_memory / size_of_unsigned_short) - size_of_unsigned_short) % 2 == 0 )
         {
          printf("\t\t\tD\n");
          divide();
         }
       else
         {
          printf("\t\t\tM\n");
          multiply();
         }
//       print_number(stdout);

       save_number_to_file();
       free(data);
       data = NULL;

       //usleep(1000*10);
    }
    // this means WHILE the result is more than one digit long
    while(size_of_allocated_memory >= size_of_unsigned_short);

  return 0;
  }
