/*
 * decrypt.c
 * 
 * Copyright 2017 Ivan Mikhailov <sinhbad@yandex.ru>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STATE_SIZE 36

/* Print array from Ath byte to Bth byte */
void print_array_bytes(uint8_t *arr, uint8_t a, uint8_t b)
{
  uint8_t i;
  uint8_t j;
  for(i = a - 1 ; i < b; i++)
  {
    printf("Byte number %d:\t", i + 1);
    for(j = 0; j < 8; j++)
    {
      printf("%d", (arr[i] & (1 << (7 - j))) >> (7 - j));
    }
    printf("\t(bits: %d - %d)\n", (i * 8 + 1), (i * 8 + 8));
  }
}

/* Print array */
void print_array(uint8_t *arr, uint8_t size_of_array)
{
  /*print_array_bytes(arr, 1, size_of_array);*/
  uint8_t i;
  printf("Initial state:\n");
  for(i = 0; i < size_of_array; i++)
  {
    printf("%02X", arr[i]);
    if((i + 1) % 10 == 0)
    {
      printf("\n");
    }
    else
    {
      printf("    ");
    } 
  }
  printf("\n");
}

/* Print array from Ath bit to Bth bit */
void print_array_bits(uint8_t *arr, uint16_t a, uint16_t b)
{
  uint8_t k;
  uint16_t i;
  for(i = a; i <= b; i++)
  {
    k = ((i-1) % 8) + 1;
    printf("%d", ((arr[(i - 1) / 8] & (1 << (8 - k))) >> (8 - k)));
  }
  printf("\t(bits: %d - %d)\n", a, b);
}

void change_bit(uint8_t *arr, uint16_t n, uint8_t value)
{
  uint8_t nbyte = (n - 1) / 8;
  uint8_t nbit = ((n - 1) % 8) + 1;

  arr[nbyte] = ((255 << (9 - nbit)) & arr[nbyte]) |
               (value << (8 - nbit)) |
               ((255 >> nbit) & arr[nbyte]);
}

uint8_t nbit(uint8_t *arr, uint16_t n)
{
  uint8_t nbyte = (n - 1) / 8;
  uint8_t nbit = ((n - 1) % 8) + 1;
  return (arr[nbyte] & (1 << (8 - nbit))) >> (8 - nbit);
}

uint8_t rotate(uint8_t *arr, uint8_t arr_size)
{
  uint8_t i;

  uint8_t a1 = nbit(arr, 91) & nbit(arr, 92);
  uint8_t a2 = nbit(arr, 175) & nbit(arr, 176);
  uint8_t a3 = nbit(arr, 286) & nbit(arr, 287);

  uint8_t t1 = nbit(arr, 66) ^ nbit(arr, 93);
  uint8_t t2 = nbit(arr, 162) ^ nbit(arr, 177);
  uint8_t t3 = nbit(arr, 243) ^ nbit(arr, 288);

  uint8_t out = t1 ^ t2 ^ t3;

  uint8_t s1 = a1 ^ nbit(arr, 171) ^ t1;
  uint8_t s2 = a2 ^ nbit(arr, 264) ^ t2;
  uint8_t s3 = a3 ^ nbit(arr, 69) ^ t3;

  /* Begin rotate */

  for(i = arr_size - 1; i > 0; i--)
  {
    arr[i] = (arr[i - 1] << 7) | (arr[i] >> 1);
  }
  arr[0] = arr[0] >> 1;

  /* End rotate */

  change_bit(arr, 1, s3);
  change_bit(arr, 94, s1);
  change_bit(arr, 178, s2);

  return out;
}

void insert_bits(uint8_t *arr, uint16_t n, uint8_t *source, uint16_t ssize)
{
  uint16_t i;
  for(i = 0; i < ssize; i++)
  {
    change_bit(arr, n + i, nbit(source, i + 1));
  }
}

void initialize_state(uint8_t *arr)
{
  uint16_t i;
  for(i = 0; i < 4*288; i++)
  {
    rotate(arr, STATE_SIZE);
  }
}

uint8_t get_byte_from_gamma(uint8_t *arr)
{
  uint8_t buf = 0;
  uint8_t i = 0;
  while(i != 8)
  {
    uint8_t z = rotate(arr, STATE_SIZE);
    buf = buf | (z << i);
    i += 1;
  }
  return buf;
}

uint8_t get_random_byte()
{
  return (uint8_t) (rand() % 256);
}

uint8_t hexchar_to_int(char ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  if (ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  if (ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  
  return -1;
}

uint8_t get_byte_from_console_input()
{
  uint8_t rb;
  uint8_t hc1, hc2;
  scanf("%c%c", &hc1, &hc2);
  rb = (hexchar_to_int(hc1) << 4) | (hexchar_to_int(hc2) );
  return rb;
}

int main(int argc, char **argv)
{
  uint8_t key[10];
  uint8_t iv[10];
  uint8_t b[STATE_SIZE];
  uint8_t buffer;
  uint8_t encbuffer;
  uint8_t i;
  FILE * pFile;
  FILE * outFile;
  for(i = 0; i < STATE_SIZE; i++) b[i] = 0;
  for(i = 0; i < 10; i++)
  {
    key[i] = 0;
  }
  printf("Trivium (decryption) by sinhbad. January, 2017.\n");
  printf("Using: trivium.exe input.file output.file\n");
  if (argc != 3)
  {
    printf("Bad input data in argv\n");
    return 0;
  }
  pFile = fopen(argv[1] , "rb");
  outFile = fopen(argv[2], "wb");
  if (pFile==NULL) {fputs ("Input file error",stderr); exit (1);}
  if (outFile==NULL) {fputs ("Output file error",stderr); exit (1);}
  for(i = 0; i < 10; i++)
  {
    fread(&buffer, 1, 1, pFile);
    iv[i] = buffer;
  }
  printf("Type key in hexadecimal format (80 bit):\n");
  for (i = 0; i < 10; i++)
  {
    key[i] = get_byte_from_console_input();
  }
  insert_bits(b, 1, key, 80);
  insert_bits(b, 94, iv, 80);
  change_bit(b, 286, 1);
  change_bit(b, 287, 1);
  change_bit(b, 288, 1);
  print_array(b, STATE_SIZE);
  initialize_state(b);
  while(fread(&buffer, 1, 1, pFile) != 0)
  {
    encbuffer = buffer ^ get_byte_from_gamma(b);
    fwrite(&encbuffer, 1, 1, outFile);
  }
  fclose(pFile);
  fclose(outFile);
  return 0;
}

