#include <stdio.h>
#include <stdlib.h>
// 'sleep()', 'usleep()'
#include <unistd.h>
// Data type 'uint8_t'
#include <stdint.h>
// Signal catching for stopping the program gracefully
#include <signal.h>
// Measuring how long the program ran
#include <time.h>

#define  ASCII_ZERO 48

#define VERBOSITY 1
#define MULTIPLE_RESULT_FILES 0

// Declare a global pointer to memory where the digits are stored
//   Since I work with digits 0-9, I only need 4 bits.
//   Though it is efficent to allow a mathematical arithmetic on those digits - like multiplication - which leads to numbers larger than a single digit.
//   Also in C language it is hard to work with less than a byte variables, so stay with 'uint8_t' to save memory, but also allow some space for the arithmetic around
static uint8_t *data = NULL;
// Counter of how many digits the large number have
//   We need this for easy iteration over those digits
static unsigned int number_of_digits;
// Carry 'bit'
//   Wherever neede in manual division or multiplication
static uint8_t carry = 0;

// In each iteration two files are used
//   From one the old_value is read, to the second the new_value is written
//   The initial file is always named 'seed'.
//   This is then changed to name 'result_x', where 'x' is the 'filename_num' variable which is incremented in each iteration
static char filename[30] = "seed";
static int filename_num = 1;

// Signal handling
static uint8_t signal_usr1 = 0;

// ------------------------------
int load_number_from_file()
  {
   // erase the carry 'bit' just to be sure
   carry = 0;

   // Open the file
   FILE *fp;
   fp = fopen(filename, "r");
     if ( fp == NULL ) { fprintf(stderr, "Unable to open file"); return 1; }

   // Find out the number of digits in the file
   //   This assumes the digits are 'char' of size 1 byte AND there is nothing but the digits in the file (e.g. no newline at the end of the file)
   fseek(fp, 0, SEEK_END);
   number_of_digits = ftell(fp);
   if ( VERBOSITY == 1 ) { printf("\tD: %d", number_of_digits); }
   if ( VERBOSITY == 2 ) { printf("\t\tNumber of digits: %d", number_of_digits); }

   data = malloc( number_of_digits * sizeof(uint8_t) );
   if ( data == NULL )
     { fprintf(stderr, "Unable to allocate enough memory. Tried to allocate %ld bytes", number_of_digits * sizeof(uint8_t) ); fclose(fp); return 1; }

   // Load the digits to the allocated memory
   fseek(fp, 0, SEEK_SET);
   for ( unsigned int i = 0 ; i < number_of_digits ; i++ )
     { data[i] = getc(fp) - ASCII_ZERO; }

  if ( fclose(fp) )
    { printf("error closing file."); exit(-1); }

   return 0;
  }

// ------------------------------
int print_number(FILE * stream)
  {
   // After multiplication we may have overflowed the the decimal order by one
   if ( carry != 0 ) fprintf(stream, "%c", carry+ASCII_ZERO);

   for ( unsigned int i = 0; i < number_of_digits ; i++)
     {
      // After division, we may have underflowed the decimal order
      if( i==0 && data[i] == 0) continue;

      fprintf( stream, "%c", data[i] + ASCII_ZERO );
     }

   return 0;
  }

int print_number_last(FILE * stream)
  {
   // After multiplication we may have overflowed the the decimal order by one
   if ( carry != 0 ) fprintf(stream, "%c", carry+ASCII_ZERO);

   for ( unsigned int i = number_of_digits - 20 ; i < number_of_digits ; i++)
     {
      // After division, we may have underflowed the decimal order
      if( i==0 && data[i] == 0) continue;

      fprintf( stream, "%c", data[i] + ASCII_ZERO );
     }

   return 0;
  }

// ------------------------------
int save_number_to_file()
  {
   FILE *fp;

   sprintf( filename, "result_%d", filename_num );

   // For large numbers, it may be VERY space consuming to save every result
   //   sometimes it might be better to disable the incrementation and stay with a single buffer file.
   if ( MULTIPLE_RESULT_FILES ) { filename_num++; }

   fp = fopen( filename, "w" );
   if ( fp == NULL )
     { fprintf(stderr, "Unable to open file '%s'", filename); return 1; }

   print_number( fp );

   if ( fclose(fp) )
     { printf("error closing file."); exit(-1); }

   return 0;
  }

// ------------------------------
// The operation is actually 'new_value = 3 * old_value + 1'
// That's why carry value is set to 1 at the beginning

int multiply()
  {
   // Setting the '+1' from the '3x+1' formula
   carry = 1;
   uint8_t value;

   for( unsigned int i = 1 ; i <= number_of_digits ; i++ )
     {
       value = data[ number_of_digits - i ];
       value = (value * 3) + carry;

       carry = value / 10;
       value = value % 10;

       data[ number_of_digits - i ] = value;
     }

   return 0;
  }

// ------------------------------
int divide()
  {
   // erase the carry 'bit'
   carry = 0;
   uint8_t value;

   for( unsigned int i = 0 ; i < number_of_digits ; i++ )
     {
      value = data[i] + ( carry * 10 );
      carry = value % 2;
      value = value / 2;

      data[i] = value;
     }

   // erase the carry 'bit' again, so no leftover is left behind for the 'print_number()' function
   carry = 0;
   return 0;
  }

// ------------------------------
void sig_handler(int signum)
  {
   fprintf(stderr, "\nA SIGNAL HAS BEEN CAUGHT\n");
   if ( signum == SIGUSR1 ) { signal_usr1++; }
  }

// ------------------------------
int main()
  {
   // Start measuring time
   time_t begin = time(NULL);


   // Register signal handler
   signal(SIGUSR1, sig_handler);

   // Iteration counter
   //   useful when we don't want to store every result, we just want to keep the final one
   #pragma GCC diagnostic error "-Wuninitialized"
   unsigned long iteration = 1;

   do
     {
      if ( VERBOSITY == 1 ) { printf("I: %lu", iteration++); }
      if ( VERBOSITY == 2 ) { printf("Iteration: %lu", iteration++); }

      load_number_from_file();
      // For numbers with milions of digits we might not want to print every one of them
//      printf("\tValue: "); print_number( stdout ); printf(" ");
        if ( VERBOSITY > 1) { printf("\t\tLast 20 digits: ..."); print_number_last( stdout ); printf(" "); }

       if( data[ number_of_digits - 1 ] % 2 == 0 )
         {
          if ( VERBOSITY == 1 ) { printf("\t D\n"); }
          if ( VERBOSITY == 2 ) { printf("\t\t D\n"); }
          divide();
         }
       else
         {
          if ( VERBOSITY == 1 ) { printf("\tM\n"); }
          if ( VERBOSITY == 2 ) { printf("\tM\n"); }
          multiply();
         }

       save_number_to_file();
       free(data);
       data = NULL;

       if ( signal_usr1 ) { fprintf(stderr, "\nSIGUSR1 has been caught, ending gracefully ...\n"); break; }

       // Artificial delay for debugging
//       usleep(1000*10);
    }
    while( number_of_digits > 1 );

   printf("\nThe elapsed time is %ld seconds\n", ( (time_t) time(NULL) - begin ) );

   return 0;
  }
