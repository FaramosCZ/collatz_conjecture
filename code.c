/*
ASCII:
  number '48' is char '0'
  number '49' is char '1'
  number '50' is char '2'
  number '51' is char '3'
  number '52' is char '4'
  number '53' is char '5'
  number '54' is char '6'
  number '55' is char '7'
  number '56' is char '8'
  number '57' is char '9'
*/

#include <stdio.h>
#include <stdlib.h>

// Declare glbal pointer to memory in which each byte will represent a char.
const unsigned short size_of_unsigned_short = sizeof(unsigned short);

unsigned short *data;
unsigned int size_of_allocated_memory;

unsigned short carry = 0;

int filename_num = 1;
char filename[200] = "seed";

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
   size_of_allocated_memory = size_of_unsigned_short * ( ftell(fp) - 0 );
   data = malloc(size_of_allocated_memory);
   if ( data == NULL )
     {
      fprintf(stderr, "Unable to allocate enough memory. Tried to allocate %d bytes", size_of_allocated_memory);
      return 1;
     }

   fseek(fp, 0, SEEK_SET);

   for(unsigned int i = 0; i < size_of_allocated_memory ; i = i + size_of_unsigned_short)
     {
      *(data+i) = getc(fp) - 48;
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
   if(carry != 0) fprintf(stream, "%c", carry+48);

   for(unsigned int i = 0; i < size_of_allocated_memory ; i = i + size_of_unsigned_short)
     {
      if( i==0 && *(data+i) == 0) continue;
      fprintf(stream, "%c", *(data+i) + 48 );
     }

   if (stream == stdout ) printf("\n");

   return 0;
  }

// ------------------------------
int multiply()
  {
   carry = 1;
   unsigned short value;

   for(unsigned int i = size_of_unsigned_short; i <= size_of_allocated_memory ; i = i + size_of_unsigned_short)
     {
       value = *(data+size_of_allocated_memory-i);
       value = (value * 3) + carry;

       carry = value / 10;
       value = value % 10;

       *(data+size_of_allocated_memory-i) = value;
     }

   return 0;
  }

// ------------------------------
int save_number_to_file()
  {
   FILE *fp;

   sprintf(filename,"result_%d",filename_num);
   filename_num++;

   fp = fopen(filename, "w");
   if ( fp == NULL )
     {
      fprintf(stderr, "Unable to open file '%s'", filename);
      return 1;
     }

   print_number(fp);
   fclose(fp);

   return 0;
  }

// ------------------------------
int divide()
  {
   carry = 0;
   unsigned short value;

   for(unsigned int i = 0; i < size_of_allocated_memory ; i = i + size_of_unsigned_short)
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

   do
     {
       load_number_from_file();
       print_number(stdout);

       if( *(data + size_of_allocated_memory - size_of_unsigned_short) % 2 == 0 )
         { divide(); }
       else
         { multiply(); }
//       print_number(stdout);

       save_number_to_file();
       free(data);
    }
    while(size_of_allocated_memory > size_of_unsigned_short);

  return 0;
  }
