/*
       /$$   /$$ /$$$$$$$$ /$$       /$$$$$$  /$$$$$$   /$$$$$$         /$$$$$$  /$$   /$$ /$$$$$$$$          
      | $$  | $$| $$_____/| $$      |_  $$_/ /$$__  $$ /$$__  $$       /$$__  $$| $$$ | $$| $$_____/          
      | $$  | $$| $$      | $$        | $$  | $$  \ $$| $$  \__/      | $$  \ $$| $$$$| $$| $$                
      | $$$$$$$$| $$$$$   | $$        | $$  | $$  | $$|  $$$$$$       | $$  | $$| $$ $$ $$| $$$$$             
      | $$__  $$| $$__/   | $$        | $$  | $$  | $$ \____  $$      | $$  | $$| $$  $$$$| $$__/             
      | $$  | $$| $$      | $$        | $$  | $$  | $$ /$$  \ $$      | $$  | $$| $$\  $$$| $$                
      | $$  | $$| $$$$$$$$| $$$$$$$$ /$$$$$$|  $$$$$$/|  $$$$$$/      |  $$$$$$/| $$ \  $$| $$$$$$$$          
      |__/  |__/|________/|________/|______/ \______/  \______/        \______/ |__/  \__/|________/          
                                                                                                              
                                                                                                              
                                                                                                              
                                       /$$     /$$                           /$$                              
                                      | $$    | $$                          |__/                              
        /$$$$$$$ /$$   /$$ /$$$$$$$  /$$$$$$  | $$$$$$$   /$$$$$$   /$$$$$$$ /$$ /$$$$$$$$  /$$$$$$   /$$$$$$ 
       /$$_____/| $$  | $$| $$__  $$|_  $$_/  | $$__  $$ /$$__  $$ /$$_____/| $$|____ /$$/ /$$__  $$ /$$__  $$
      |  $$$$$$ | $$  | $$| $$  \ $$  | $$    | $$  \ $$| $$$$$$$$|  $$$$$$ | $$   /$$$$/ | $$$$$$$$| $$  \__/
       \____  $$| $$  | $$| $$  | $$  | $$ /$$| $$  | $$| $$_____/ \____  $$| $$  /$$__/  | $$_____/| $$      
       /$$$$$$$/|  $$$$$$$| $$  | $$  |  $$$$/| $$  | $$|  $$$$$$$ /$$$$$$$/| $$ /$$$$$$$$|  $$$$$$$| $$      
      |_______/  \____  $$|__/  |__/   \___/  |__/  |__/ \_______/|_______/ |__/|________/ \_______/|__/      
                 /$$  | $$                                                                                    
                |  $$$$$$/                                                                                    
                 \______/     
                                                                
                                                                                 v4.6 

   
 // A BlogHoskins Monstrosity @ 2019 / 2020
// https://bloghoskins.blogspot.com/
/*    
 *    v5.0
 *    Allow up to three notes to be played simultaniously 
 *    PLAYABLE_NOTES constant controls this. more than 3 doesn't seem to work on an arduino nanao
 *    v4.7
 *    Fix midi keyboard ghosting
 *    v4.6 
 *    Time to fix distortion - Fixed
 *    changed update audio from 8 -> 9
 *    Change Filter Res and Cut-off from >>2 to mozzieMap -> done
 *    change lfo amount pot - check if all are still in spec with multimeter - done
 *    add resistors? - yep, done & done
 *    reduce resonance total in res pot - done
 *    
 *    v4.5
 *    LFO On/off switch added - using arduino digital pin 3
 *     
 *    v4.4 
 *    Applied advice given from Mozzi Forums - working much better now - thanks Staffan
 *    Still need to apply values to stop distortion + add on/off switch for LFO
 *     
 *    v4.3 LFO implementation
 *    LFO is working(?), but filter is interfering.  Figure it out!
 *    *replaced pin assignments 'const int' with #define -> now reassign values to stop distortion
 *    
 *    
 *    V3.1 - Minor Updates
 *   *IImproved Filter Resonance (?).  Apparantly After setting resonance, you need to call setCuttoffFreq() to hear the change.
 *   Swapped order of cut-off and resonance in code.  Filter Sounds Better now?
 *   *Increased note sustain from 10 seconds to 60 seconds
 *   *OSC OUTPUT made louder on audio update>>9; // changed down from 10 to 9 because was louder
 *   
 *   
 *   V3
 *   In this version we add a low pass filter.  A cut off pot and resonance pot are added.
 *   For this you'll need two B10k pots, and a 220ohm resistor.
 *   A3: for Resonance (center pot).  Other lugs go to ground and +5v
 *   A2: for Cut-off (center pot).   Other lugs go to ground (with 220ohm resistor) and +5v
 *   
 *   
 *   v2
 *   https://bloghoskins.blogspot.com/2020/06/helios-one-arduino-synth-part-2.html
 *   This version adds Attack & Release on the analog inputs. You'll need 2 pots. 
 *   Connect center pot pin to;
 *   A5: for Atk
 *   A4: for Release
 *   connect the other pot pins to ground and 5v. 
 *   To stop mis-triggering on the atk I have x2 1k resistors in parallel (amounting 
 *   to 500 ohms resistance) on the ground input of the atk pot. you could put two 
 *   200(ish)ohm resisters in series instead, or play with the code...  maybe set the
 *   int val of the atkVal to something over 20 instead of 0?
 *   
 *   
 *   v1
 *   http://bloghoskins.blogspot.com/2019/07/helios-one-arduino-synth-part-1.html
 *   This vesrion of code lets you set between SQR and SAW wave with a 
 *   switch (input 2 on arduino)
 *   MIDI is working.
 *   You'll need to install the MIDI & Mozzi arduino libraries to get it to work.
*/


#include <MIDI.h>
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <mozzi_midi.h>
#include <ADSR.h>
#include <LowPassFilter.h> // You need this for the LPF
#include <AutoMap.h> // to track and reassign the pot values

#include <tables/saw2048_int8.h> // saw table for oscillator
#include <tables/square_no_alias_2048_int8.h> // square table for oscillator

//*****************LFO******************************************************************************************
#include <tables/cos2048_int8.h> // for filter modulation
#include <StateVariable.h>
#include <mozzi_rand.h> // for rand()
#include <AutoMap.h> // To track pots **************************************************************************

//********ADD LFO FILTER MODULATION OSC*************************************************************************
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kFilterMod(COS2048_DATA);
StateVariable <NOTCH> svf; // can be LOWPASS, BANDPASS, HIGHPASS or NOTCH

//// Set up LFO Rate Pot****************************************************************************************
#define LFOratePot A1    // select the input pin for the potentiometer
AutoMap LFOratePotMap(0, 1023, 40, 750);  // LFO Rate mapped 25 to 1300 (up from 5 - 1023)
//***************END********************************************************************************************

//// ****************Set up LFO Res Pot*************************************************************************
#define LFOresPot A0    // select the input pin for the potentiometer
AutoMap LFOresPotMap(0, 1023, 40, 180);  // 0-255 val distorted, 2-212 within range - find better value?
//***************END********************************************************************************************

//Create an instance of a low pass filter
LowPassFilter lpf; 

MIDI_CREATE_DEFAULT_INSTANCE();

#define LED 5 // Internal LED lights up if MIDI is being received

#define WAVE_SWITCH 2 // switch for switching waveforms
#define LFO_ON_OFF_SWITCH 3 // switch for LFO ON/OFF

// Set up Attack & Decay Envelope
#define atkPot A5    // select the input pin for the potentiometer
AutoMap atkPotMap(0, 1023, 0, 3000);  // remap the atk pot to 3 seconds

#define dkyPot A4    // select the input pin for the potentiometer
AutoMap dkyPotMap(0, 1023, 0, 3000);  // remap the atk pot to 3 seconds

//*******CUT-OFF POT***********
#define cutoffPot A2    // cut-off pot will be on A2
AutoMap cutOffPotMap(0, 1023, 20, 250);  // 0 - 255

//*******RESONANCE POT***********
#define resPot A3    // resonance pot will be on A2
AutoMap resFreqPotMap(0, 1023, 40, 210);  // 0 - 255

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 128 // powers of 2 please

// number of playable notes at once. 3 seems to be the max for my arduino nano;
const int PLAYABLE_NOTES = 3 ;

// audio sinewave oscillators
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> oscilators[PLAYABLE_NOTES];

// envelope generators
ADSR <CONTROL_RATE, AUDIO_RATE> envelopes[PLAYABLE_NOTES];


//keep track of which notes are currently being played and where
byte PLAYING[PLAYABLE_NOTES];

//keep track of which is the oldest note played
int played_order[PLAYABLE_NOTES];
int played_index = 0;

void HandleNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(LED,HIGH);
  
  //remove any notes that have stopped playing;
  for ( int i =0; i < PLAYABLE_NOTES; i++ ) {
    if (  ! envelopes[i].playing() ) { //turn off the note for envelopes
       PLAYING[i] = NULL;
    }
  } 
  
  //first check if there is an oscilator already playing that note
  for ( int i =0; i < PLAYABLE_NOTES; i++ ) {
    if ( note == PLAYING[i] ) { //just use the same oscilator
      played_order[played_index] = i;
      played_index = (played_index + 1 ) % PLAYABLE_NOTES;
      envelopes[i].noteOn();
      return;
    }
  }
  
  //find an open oscilator and set its frequency
  for( int i = 0; i < PLAYABLE_NOTES; i++ ) { 
    if ( PLAYING[i] == NULL  && !envelopes[i].playing()) { 
      PLAYING[i] = note;
      oscilators[i].setFreq(mtof(float(note)));
      envelopes[i].noteOn();
      
      played_order[played_index] = i;
      played_index = (played_index + 1 ) % PLAYABLE_NOTES;
      
      return;
    }
  }
  
  //no open oscilator, so use the oldest one
  int i = played_order[ ( played_index + 1 ) % PLAYABLE_NOTES ];
  envelopes[i].noteOff();
  oscilators[i].setFreq(mtof(float(note)));
  envelopes[i].noteOn();
  PLAYING[i] = note;

  played_order[played_index] = i;
  played_index = (played_index + 1 ) % PLAYABLE_NOTES;
  
}

void HandleNoteOff(byte channel, byte note, byte velocity) { 
  
  for( int i = 0; i < PLAYABLE_NOTES; i++ ) { 
    if ( PLAYING[i] == note) { 
      envelopes[i].noteOff();
    }
  }

  
  digitalWrite(LED,LOW);
  
  
}

void setup() {
//  Serial.begin(9600); // see the output
  pinMode(LED, OUTPUT);  // built-in arduino led lights up with midi sent data 
  digitalWrite(LED,HIGH);
  delay(100);
  digitalWrite(LED,LOW);
  delay(100);
  digitalWrite(LED,HIGH);
  delay(100);
  digitalWrite(LED,LOW);
  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function
  // Initiate MIDI communications, listen to all channels (not needed with Teensy usbMIDI)
  MIDI.begin(MIDI_CHANNEL_OMNI);  
  
   // A bit of attack / decay while testing
  
  for( int i = 0; i < PLAYABLE_NOTES; i++ ) { 
    oscilators[i].setFreq(440); // default frequency
    envelopes[i].setADLevels(255,64);
  }

//****************Set up LFO**************************************************************************************
  kFilterMod.setFreq(1.3f); // Can be deleted??
//****************End*********************************************************************************************
  pinMode(2, INPUT_PULLUP); // Pin two is connected to the middle of a switch, high switch goes to 5v, low to ground
  pinMode(3, INPUT_PULLUP); // Pin two is connected to the middle of a switch, high switch goes to 5v, low to ground
  startMozzi(CONTROL_RATE); 
}


void updateControl(){
  MIDI.read();
  // set ADSR times
  int atkVal = mozziAnalogRead(atkPot);    // read the value from the attack pot
  atkVal = atkPotMap(atkVal);
  int dkyVal = mozziAnalogRead(dkyPot);    // read the value from the decay pot
  dkyVal = dkyPotMap(dkyVal);
  for( int i = 0; i< PLAYABLE_NOTES; i++ ) {
    envelopes[i].setTimes(atkVal,60000,60000,dkyVal); // 60000 is so the note will sustain 60 seconds unless a noteOff comes
    envelopes[i].update();
  }

  //**************RESONANCE POT****************
  int resVal = mozziAnalogRead(resPot);  // arduino checks pot value
//  uint8_t res_freq = resVal >> 2; // ***8 bit unsigned to store 0-1023 / 4 
  resVal = resFreqPotMap(resVal);
  lpf.setResonance(resVal);  // change the freq

  //**************CUT-OFF POT****************
  int cutVal = mozziAnalogRead(cutoffPot);  // arduino checks pot value
//  uint8_t cutoff_freq = cutVal >> 2; // ***8 bit unsigned to store 0-1023 / 4
  cutVal = cutOffPotMap(cutVal);
  lpf.setCutoffFreq(cutVal);  // change the freq
  
  //*****************Set up LFO control pots****************************************************************************
  int LFOrateVal = mozziAnalogRead(LFOratePot);    // Speed of LFO
  LFOrateVal = LFOratePotMap(LFOrateVal);         // map to 25-1300

  int LFOresVal = mozziAnalogRead(LFOresPot);    // read the value from the attack pot
  LFOresVal = LFOresPotMap(LFOresVal);            // map to 10-210

//*****************************ADD LFO**********************************************************************************
  int LFOsensorVal = digitalRead(3); // read the switch position value into a  variable
  if (LFOsensorVal == HIGH) // If switch is set to high, run this portion of code
  {
    if (rand(CONTROL_RATE/2) == 0){ // about once every half second
    kFilterMod.setFreq(LFOrateVal/64);  // choose a new modulation frequency****Replace setFreq((float)rand(255)/64)****
  }
  int LFOrate_freq = 2200 + kFilterMod.next()*12; // cos oscillator, 2200 + (int8)*12
  svf.setCentreFreq(LFOrate_freq);
  svf.setResonance(LFOresVal); 
  }
  else  // If switch not set to high, run this portion of code instead
  {
  svf.setCentreFreq(0);
  svf.setResonance(200); 
  }
  //**********************************END*******************************************************************************
  
  int sensorVal = digitalRead(2); // read the switch position value into a  variable
  
  for( int i = 0; i < PLAYABLE_NOTES; i++ ) { 
      if (sensorVal == HIGH) // If switch is set to high, run this portion of code
      {
        oscilators[i].setTable(SAW2048_DATA);
      } else { 
        oscilators[i].setTable(SQUARE_NO_ALIAS_2048_DATA);  
      }
  }
  
}

int playing = 0;

int updateAudio(){

    //add up the playing oscilators
    int oscil = 0 ;
    for( int i = 0; i < PLAYABLE_NOTES; i++ ) { 
      oscil = oscil +  (( oscilators[i].next() * envelopes[i].next() ) >> 10);
    }
   
    int asig =  oscil; // multiplying by 0-255 and then dividing by 256
    int asigSVF = svf.next(asig); // SVF
    int asigLPF = lpf.next(asigSVF); // LPF
    return (asigLPF); 
}

void loop() {
  audioHook(); // required here
} 
