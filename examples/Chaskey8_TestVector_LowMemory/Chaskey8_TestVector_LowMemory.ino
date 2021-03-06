/*
   Chaskey C implementation (speed optimized)

   Written in 2014 by Nicky Mouha, based on SipHash
   Modified in 2017 by Aaron Lindsay
   Modified in 2021 by E for simple library use on microcontroller boards

   Tested on Arduino (Uno, Mega 2560), NodeMCU ESP8266, ESP32 DEVKIT V1 DOIT.
   Build and tested on Arduino IDE 1.8.7

   To the extent possible under law, the author has dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
   
   NOTE: This implementation assumes a little-endian architecture
   that does not require aligned memory accesses.

   DESCRIPTION: This example runs tests on the Chaskey 8 Rounds implementation to verify correct behaviour
   based on test vectors available at https://mouha.be/wp-content/uploads/chaskey-speed.c
   Block of Test Vector Result has been modified to uint8_t format to get low memory use.
*/

#include <chaskey.h>

const uint8_t vectors[64][8] =
{
  { 0xe5, 0x8f, 0x2e, 0x79, 0xaa, 0x87, 0xce, 0x75 },
  { 0x7b, 0x30, 0xa9, 0x13, 0x89, 0x2c, 0xe6, 0x50 },
  { 0x22, 0x89, 0xdf, 0x55, 0x77, 0xf5, 0x7f, 0x2c },
  { 0x64, 0xb2, 0xdb, 0x1b, 0xd8, 0x80, 0x76, 0xa0 },
  { 0x71, 0xd1, 0xb2, 0x30, 0xfb, 0x32, 0x85, 0xe3 },
  { 0x0c, 0x3d, 0x98, 0xbc, 0x64, 0x40, 0xb1, 0x31 },
  { 0x8a, 0x68, 0xd0, 0x0d, 0x6c, 0x75, 0x31, 0xe1 },
  { 0x54, 0x04, 0x67, 0x7f, 0xe0, 0x03, 0x5b, 0xf2 },
  { 0x69, 0x0f, 0x33, 0x09, 0xe0, 0xdc, 0xb5, 0x62 },
  { 0xbe, 0xb1, 0xb3, 0x89, 0x92, 0x73, 0xb9, 0x95 },
  { 0xae, 0x9d, 0x5b, 0xac, 0xac, 0xc0, 0xf8, 0x6c },
  { 0xec, 0xdb, 0xb0, 0xd5, 0x30, 0x25, 0x69, 0xc1 },
  { 0x91, 0x33, 0x2c, 0xfc, 0xd5, 0x8c, 0x5c, 0x28 },
  { 0x33, 0x6f, 0x49, 0x29, 0x58, 0xd5, 0x62, 0xac },
  { 0x97, 0x84, 0x66, 0xbf, 0xa1, 0x17, 0x52, 0x27 },
  { 0xa4, 0x4d, 0xb9, 0x51, 0xe8, 0x4d, 0xcc, 0xef },
  { 0xa9, 0x1c, 0x27, 0x79, 0x71, 0x1c, 0x6a, 0xd6 },
  { 0x68, 0xa9, 0x8d, 0x04, 0x96, 0xd0, 0x25, 0x4e },
  { 0x80, 0xd3, 0x45, 0x0c, 0x96, 0x99, 0xd0, 0x2f },
  { 0x72, 0x34, 0x15, 0xd8, 0x1e, 0x7b, 0xc3, 0x10 },
  { 0x43, 0xa5, 0x4c, 0xfa, 0x1e, 0xd7, 0x75, 0x0d },
  { 0xca, 0x1b, 0x8b, 0x80, 0xe0, 0x4d, 0x03, 0x7e },
  { 0x41, 0xa4, 0xaf, 0xc7, 0xed, 0xef, 0xa4, 0x95 },
  { 0x41, 0x06, 0x20, 0x36, 0x4a, 0x1f, 0x8c, 0x2f },
  { 0x35, 0x1e, 0xba, 0x37, 0x62, 0x1a, 0x45, 0x43 },
  { 0x97, 0xf6, 0xb4, 0x86, 0x4f, 0xf6, 0xa4, 0x93 },
  { 0xfa, 0x2a, 0x7d, 0xbe, 0xe7, 0x3d, 0x51, 0xac },
  { 0x54, 0x7f, 0x6d, 0xc5, 0x58, 0x6a, 0x28, 0x3e },
  { 0xed, 0x08, 0x0f, 0x3d, 0xde, 0x3f, 0x2e, 0xf3 },
  { 0xf8, 0x71, 0xc1, 0x2e, 0x09, 0x83, 0x69, 0x33 },
  { 0xac, 0xee, 0xec, 0x5c, 0x4c, 0x08, 0x74, 0xa1 },
  { 0x2d, 0x0c, 0xdd, 0xbb, 0xd9, 0xfc, 0xb6, 0xfa },
  { 0xaf, 0xf7, 0xb3, 0x60, 0xc8, 0xe7, 0xee, 0x37 },
  { 0x33, 0xea, 0x44, 0xdf, 0x98, 0xc3, 0xb2, 0xb0 },
  { 0x75, 0x11, 0xe3, 0xc7, 0x4d, 0xe3, 0xb4, 0x6d },
  { 0x38, 0x69, 0xdc, 0xe0, 0xe3, 0xa7, 0xa0, 0x84 },
  { 0x66, 0x6c, 0xeb, 0x1c, 0x74, 0xf2, 0x35, 0x35 },
  { 0xf4, 0x06, 0xa1, 0xbb, 0x7c, 0x69, 0x9b, 0xd4 },
  { 0x39, 0x8a, 0xd5, 0x5a, 0x44, 0x28, 0xd5, 0xdf },
  { 0x1f, 0x7b, 0xa6, 0x67, 0xb3, 0xec, 0x75, 0x35 },
  { 0x27, 0xcc, 0xab, 0xd5, 0xf5, 0xef, 0x14, 0x91 },
  { 0x49, 0xdf, 0x59, 0xb5, 0xcf, 0xb2, 0xc9, 0xde },
  { 0x29, 0x72, 0xca, 0x2a, 0x77, 0x1b, 0xff, 0x99 },
  { 0xfd, 0x96, 0x59, 0x56, 0xef, 0x8c, 0x98, 0x8f },
  { 0x47, 0x37, 0x47, 0xbe, 0x7b, 0x82, 0x90, 0x25 },
  { 0x7d, 0xab, 0x60, 0xf8, 0x88, 0x8c, 0xf4, 0x00 },
  { 0xe1, 0xc7, 0x15, 0xde, 0xf8, 0xef, 0x90, 0x1d },
  { 0xc3, 0xa2, 0xf0, 0xb3, 0xa7, 0x39, 0x55, 0x77 },
  { 0x21, 0x6e, 0x7c, 0x12, 0x59, 0xa4, 0x07, 0x6c },
  { 0x32, 0xf1, 0xf3, 0x08, 0xe3, 0x87, 0xb5, 0x57 },
  { 0x24, 0xe8, 0x26, 0xa8, 0x6a, 0x1e, 0x85, 0x3f },
  { 0x3a, 0xa1, 0xa6, 0x14, 0xfd, 0x62, 0x99, 0x46 },
  { 0xf7, 0xdd, 0x20, 0xfe, 0x29, 0x52, 0x50, 0x06 },
  { 0x3c, 0xa3, 0xe7, 0x1d, 0x96, 0x1c, 0xf8, 0x37 },
  { 0x9a, 0x1e, 0xd0, 0x5f, 0x6d, 0x48, 0x2e, 0x9f },
  { 0xa5, 0x5c, 0x7f, 0xe1, 0xd0, 0xbd, 0xd4, 0x37 },
  { 0xae, 0xee, 0x7c, 0x81, 0xc0, 0x9e, 0x6c, 0x79 },
  { 0x63, 0x7e, 0x82, 0xb7, 0xa0, 0xfe, 0x88, 0x09 },
  { 0x4b, 0x8d, 0x24, 0xf0, 0xc8, 0xbd, 0xa7, 0xac },
  { 0xb6, 0x3e, 0x36, 0x67, 0x47, 0xe0, 0xe8, 0xfa },
  { 0x15, 0x1d, 0xbd, 0x3d, 0x7b, 0x2d, 0x09, 0x05 },
  { 0x02, 0x91, 0xf3, 0xeb, 0x08, 0x17, 0x4c, 0x8f },
  { 0x54, 0xd4, 0xa0, 0x89, 0x82, 0xa2, 0x01, 0x92 },
  { 0xd7, 0xfa, 0x47, 0x90, 0x8c, 0x6d, 0x13, 0x88 }
};
uint8_t m[64];          //Message to process
uint8_t tag[16];        //Chaskey MAC Result
uint32_t k[4] = {0x833D3433, 0x009F389F, 0x2398E64F, 0x417ACF39}; //Key used
uint32_t k1[4], k2[4];  //Subkeys used
uint32_t taglen = 8;   //Chaskey MAC Length
uint32_t vector[4];

void setup() {
  Serial.begin(9600);
  Serial.println("8-Round Chaskey Algorithm Test Vector..\n");

  test_vector();
}

void loop() {

}

void test_vector(){
  chaskey_subkeys(k1, k2, k);

  for(int i=0; i<64; i++){
    m[i] = i;
    chaskey(tag, taglen, m, i, k, k1, k2);
    
    String str_hasil = "";
    String str_vector = "";
    String chaskey8;
    String vector_hasil;
    byte buf_hasil;
    byte buf_vector;
    char ch_hasil[16];
    char ch_vector[16];
    str_hasil = tag;
    str_hasil.toCharArray(ch_hasil,16);
    
    for(int a=0; a<8; a++){
      buf_hasil = ch_hasil[a];
      //Serial.print(buf_hasil, HEX);
      chaskey8.concat(String(buf_hasil < 0x10 ? "0" : ""));
      chaskey8.concat(String(buf_hasil,HEX));
      chaskey8.toUpperCase();
    }

    str_vector = (char *)vectors[i];
    str_vector.toCharArray(ch_vector,16);
    
    for(int b=0; b<8; b++){
      buf_vector = ch_vector[b];
      //Serial.print(buf_vector, HEX);
      vector_hasil.concat(String(buf_vector < 0x10 ? "0" : ""));
      vector_hasil.concat(String(buf_vector,HEX));
      vector_hasil.toUpperCase();
    }
    
    Serial.print("Chaskey: ");
    Serial.print(chaskey8);
    Serial.print("\tTest Vector: ");
    Serial.println(vector_hasil);
    chaskey8 = "";
    vector_hasil = "";

    memcpy(vector, vectors[i], sizeof(vector));
    if(memcmp(tag, vector, taglen)){
      Serial.println("TEST VECTOR FAILED IN THIS SECTION!\n");
    }
    }
    Serial.println("TEST VECTOR COMPLETE!\n");
}
