#include <iostream>
#include <math.h>
#include "sintesisFM.h"
#include "keyvalue.h"

#include <stdlib.h>

using namespace upc;
using namespace std;

SintesisFM::SintesisFM(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  /*
    You can use the class keyvalue to parse "param" and configure your instrument.
    Take a Look at keyvalue.h    
  */
  KeyValue kv(param);
  int N;

  if (!kv.to_int("N",N))
    N = 40; //default value
  
  /*if (!kv.to_float("I",I))
    I = 1; //default value

  I = 1. - pow(2, -I / 12.);

  if (!kv.to_float("fm",fm))
    fm = 8; //default value
  
  fase_sen = 0;
  fase_mod = 0;
  inc_fase_mod = (2*M_PI*fm) / SamplingRate;*/
  
  //Create a tbl with one period of a sinusoidal wave
  tbl.resize(N);
  float phase = 0, step = 2 * M_PI /(float) N;
  index = 0;
  for (int i=0; i < N ; ++i) {
    tbl[i] = sin(phase);
    phase += step;
  }
}


void SintesisFM::command(long cmd, long note, long vel) {
  if (cmd == 9) {		//'Key' pressed: attack begins
    bActive = true;
    adsr.start();
    index = 0;
	A = vel / 127.;
    fase = 0;
    f0 = pow(2,(note-69.)/12.)*440;
    nota = (tbl.size()/(double) SamplingRate)*f0;

    //s1 = 1 - I*sin(fase_mod);
    //s2 = inc_fase_mod;
  }
  else if (cmd == 8) {	//'Key' released: sustain ends, release begins
    adsr.stop();
  }
  else if (cmd == 0) {	//Sound extinguished without waiting for release to end
    adsr.end();
  }
}


const vector<float> & SintesisFM::synthesize() {
  if (not adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (not bActive)
    return x;

  for (unsigned int i=0; i<x.size(); ++i) {
    x[i] = A * tbl[index++];
    if (index == tbl.size())
      index = 0;
    /*fase_sen = fmod(fase_sen+s1,tbl.size());
    fase_mod = fmod(fase_mod+s2,tbl.size());
    index = floor(fase);*/
    //x[i] = 
  }
  adsr(x); //apply envelope to x and update internal status of ADSR

  return x;
}