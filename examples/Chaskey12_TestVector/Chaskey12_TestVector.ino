/*
   Chaskey-12 reference C implementation

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

   DESCRIPTION: This example runs tests on the Chaskey 12 Rounds implementation to verify correct behaviour
   based on test vectors available at https://mouha.be/wp-content/uploads/chaskey12.c
*/

#include <chaskey12.h>

const uint8_t vectors[64][8] =
{
  { 0xdd, 0x3e, 0x18, 0x49, 0xd6, 0x82, 0x45, 0x55 },
  { 0xed, 0x1d, 0xa8, 0x9e, 0xc9, 0x31, 0x79, 0xca },
  { 0x98, 0xfe, 0x20, 0xa3, 0x43, 0xcd, 0x66, 0x6f },
  { 0xf6, 0xf4, 0x18, 0xac, 0xdd, 0x7d, 0x9f, 0xa1 },
  { 0x4c, 0xf0, 0x49, 0x60, 0x09, 0x99, 0x49, 0xf3 },
  { 0x75, 0xc8, 0x32, 0x52, 0x65, 0x3d, 0x3b, 0x57 },
  { 0x96, 0x4b, 0x04, 0x61, 0xfb, 0xe9, 0x22, 0x73 },
  { 0x14, 0x1f, 0xa0, 0x8b, 0xbf, 0x39, 0x96, 0x36 },
  { 0x41, 0x2d, 0x98, 0xed, 0x93, 0x6d, 0x4a, 0xb2 },
  { 0xfb, 0x0d, 0x98, 0xbc, 0x70, 0xe3, 0x05, 0xf9 },
  { 0x36, 0xf8, 0x8e, 0x1f, 0xda, 0x86, 0xc8, 0xab },
  { 0x4d, 0x1a, 0x18, 0x15, 0x86, 0x8a, 0x5a, 0xa8 },
  { 0x7a, 0x79, 0x12, 0xc1, 0x99, 0x9e, 0xae, 0x81 },
  { 0x9c, 0xa1, 0x11, 0x37, 0xb4, 0xa3, 0x46, 0x01 },
  { 0x79, 0x05, 0x14, 0x2f, 0x3b, 0xe7, 0x7e, 0x67 },
  { 0x6a, 0x3e, 0xe3, 0xd3, 0x5c, 0x04, 0x33, 0x97 },
  { 0xd1, 0x39, 0x70, 0xd7, 0xbe, 0x9b, 0x23, 0x50 },
  { 0x32, 0xac, 0xd9, 0x14, 0xbf, 0xda, 0x3b, 0xc8 },
  { 0x8a, 0x58, 0xd8, 0x16, 0xcb, 0x7a, 0x14, 0x83 },
  { 0x03, 0xf4, 0xd6, 0x66, 0x38, 0xef, 0xad, 0x8d },
  { 0xf9, 0x93, 0x22, 0x37, 0xff, 0x05, 0xe8, 0x31 },
  { 0xf5, 0xfe, 0xdb, 0x13, 0x48, 0x62, 0xb4, 0x71 },
  { 0x8b, 0xb5, 0x54, 0x86, 0xf3, 0x8d, 0x57, 0xea },
  { 0x8a, 0x3a, 0xcb, 0x94, 0xb5, 0xad, 0x59, 0x1c },
  { 0x7c, 0xe3, 0x70, 0x87, 0x23, 0xf7, 0x49, 0x5f },
  { 0xf4, 0x2f, 0x3d, 0x2f, 0x40, 0x57, 0x10, 0xc2 },
  { 0xb3, 0x93, 0x3a, 0x16, 0x7e, 0x56, 0x36, 0xac },
  { 0x89, 0x9a, 0x79, 0x45, 0x42, 0x3a, 0x5e, 0x1b },
  { 0x65, 0xe1, 0x2d, 0xf5, 0xa6, 0x95, 0xfa, 0xc8 },
  { 0xb8, 0x24, 0x49, 0xd8, 0xc8, 0xa0, 0x6a, 0xe9 },
  { 0xa8, 0x50, 0xdf, 0xba, 0xde, 0xfa, 0x42, 0x29 },
  { 0xfd, 0x42, 0xc3, 0x9d, 0x08, 0xab, 0x71, 0xa0 },
  { 0xb4, 0x65, 0xc2, 0x41, 0x26, 0x10, 0xbf, 0x84 },
  { 0x89, 0xc4, 0xa9, 0xdd, 0xb5, 0x3e, 0x69, 0x91 },
  { 0x5a, 0x9a, 0xf9, 0x1e, 0xb0, 0x95, 0xd3, 0x31 },
  { 0x8e, 0x54, 0x91, 0x4c, 0x15, 0x1e, 0x46, 0xb0 },
  { 0xfa, 0xb8, 0xab, 0x0b, 0x5b, 0xea, 0xae, 0xc6 },
  { 0x60, 0xad, 0x90, 0x6a, 0xcd, 0x06, 0xc8, 0x23 },
  { 0x6b, 0x1e, 0x6b, 0xc2, 0x42, 0x6d, 0xad, 0x17 },
  { 0x90, 0x32, 0x8f, 0xd2, 0x59, 0x88, 0x9a, 0x8f },
  { 0xf0, 0xf7, 0x81, 0x5e, 0xe6, 0xf3, 0xd5, 0x16 },
  { 0x97, 0xe7, 0xe2, 0xce, 0xbe, 0xa8, 0x26, 0xb8 },
  { 0xb0, 0xfa, 0x18, 0x45, 0xf7, 0x2a, 0x76, 0xd6 },
  { 0xa4, 0x68, 0xbd, 0xfc, 0xdf, 0x0a, 0xa9, 0xc7 },
  { 0xda, 0x84, 0xe1, 0x13, 0x38, 0x38, 0x7d, 0xa7 },
  { 0xb3, 0x0d, 0x5e, 0xad, 0x8e, 0x39, 0xf2, 0xbc },
  { 0x17, 0x8a, 0x43, 0xd2, 0xa0, 0x08, 0x50, 0x3e },
  { 0x6d, 0xfa, 0xa7, 0x05, 0xa8, 0xa0, 0x6c, 0x70 },
  { 0xaa, 0x04, 0x7f, 0x07, 0xc5, 0xae, 0x8d, 0xb4 },
  { 0x30, 0x5b, 0xbb, 0x42, 0x0c, 0x5d, 0x5e, 0xcc },
  { 0x08, 0x32, 0x80, 0x31, 0x59, 0x75, 0x0f, 0x49 },
  { 0x90, 0x80, 0x25, 0x4f, 0xb7, 0x9b, 0xab, 0x1a },
  { 0x61, 0xc2, 0x85, 0xca, 0x24, 0x57, 0x74, 0xa4 },
  { 0x2a, 0xae, 0x03, 0x5c, 0xfb, 0x61, 0xf9, 0x7a },
  { 0xf5, 0x28, 0x90, 0x75, 0xc9, 0xab, 0x39, 0xe5 },
  { 0xe6, 0x5c, 0x42, 0x37, 0x32, 0xda, 0xe7, 0x95 },
  { 0x4b, 0x22, 0xcf, 0x0d, 0x9d, 0xa8, 0xde, 0x3d },
  { 0x26, 0x26, 0xea, 0x2f, 0xa1, 0xf9, 0xab, 0xcf },
  { 0xd1, 0xe1, 0x7e, 0x6e, 0xc4, 0xa8, 0x8d, 0xa6 },
  { 0x16, 0x57, 0x44, 0x28, 0x27, 0xff, 0x64, 0x0a },
  { 0xfd, 0x15, 0x5a, 0x40, 0xdf, 0x15, 0xf6, 0x30 },
  { 0xff, 0xeb, 0x59, 0x6f, 0x29, 0x9f, 0x58, 0xb2 },
  { 0xbe, 0x4e, 0xe4, 0xed, 0x39, 0x75, 0xdf, 0x87 },
  { 0xfc, 0x7f, 0x9d, 0xf7, 0x99, 0x1b, 0x87, 0xbc }
};
uint8_t m[64];          //Message to process
uint8_t tag[16];        //Chaskey MAC Result
uint8_t k[16] = {0x00, 0x11, 0x22, 0x33,
                 0x44, 0x55, 0x66, 0x77,
                 0x88, 0x99, 0xaa, 0xbb,
                 0xcc, 0xdd, 0xee, 0xff};   //Key used
uint32_t k1[4], k2[4];  //Subkeys used
uint32_t taglen = 8;    //Chaskey MAC Length
uint32_t vector[4];

void setup() {
  Serial.begin(9600);
  Serial.println("12-Round Chaskey Algorithm Test Vector..\n");

  test_vector();
}

void loop() {
  
}

void test_vector(){
  chaskey12_subkeys(k1, k2, (uint32_t*) k);

  for(int i=0; i<64; i++){
    m[i] = i;
    chaskey12(tag, taglen, m, i, (uint32_t*) k, k1, k2);

    String str_hasil = "";
    String str_vector = "";
    String chaskey12;
    String vector_hasil;
    byte buf_hasil;
    byte buf_vector;
    char ch_hasil[16];
    char ch_vector[16];
    str_hasil = (const char*)tag;
    str_hasil.toCharArray(ch_hasil,16);
    
    for(int a=0; a<8; a++){
      buf_hasil = ch_hasil[a];
      //Serial.print(buf_hasil, HEX);
      chaskey12.concat(String(buf_hasil < 0x10 ? "0" : ""));
      chaskey12.concat(String(buf_hasil,HEX));
      chaskey12.toUpperCase();
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
    
    Serial.print("Chaskey12: ");
    Serial.print(chaskey12);
    Serial.print("\tTest Vector: ");
    Serial.println(vector_hasil);
    chaskey12 = "";
    vector_hasil = "";

    memcpy(vector, vectors[i], sizeof(vector));
    if(memcmp(tag, vector, taglen)){
      Serial.println("TEST VECTOR FAILED IN THIS SECTION!\n");
    }
    }
    Serial.println("TEST VECTOR COMPLETE!\n");
}
