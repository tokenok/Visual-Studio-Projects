#pragma once

#pragma comment(lib, "winmm.lib")
#include "RtMidi.h"

#include <thread>
#include <string>
#include <map>

//https://www.music.mcgill.ca/~gary/rtmidi/index.html#virtual
//http://www.music-software-development.com/midi-tutorial.html

//1 - 8		Piano
//9 - 16	Chromatic Percussion
//17 - 24	Organ
//25 - 32	Guitar
//33 - 40	Bass
//41 - 48	Strings
//49 - 56	Ensemble
//57 - 64	Brass
//65 - 72	Reed
//73 - 80	Pipe
//81 - 88	Synth Lead
//89 - 96	Synth Pad
//97 - 104	Synth Effects
//105 - 112	Ethnic
//113 - 120	Percussive
//121 - 128	Sound Effects

extern std::map<unsigned, std::wstring> InstrumentNames;

enum Drum{
	Acoustic_Bass_Drum = 35,Bass_Drum_,Side_Stick,Acoustic_Snare,Hand_Clap,Electric_Snare,Low_Floor_Tom,Closed_Hi_Hat,
	High_Floor_Tom,Pedal_HiHat,Low_Tom,Open_HiHat,LowMid_Tom,HiMid_Tom,Crash_Cymbal_1,High_Tom,
	Ride_Cymbal_1,Chinese_Cymbal,Ride_Bell,Tambourine,Splash_Cymbal,Cowbell,Crash_Cymbal_2,Vibraslap,
	Ride_Cymbal_2,Hi_Bongo,Low_Bongo,Mute_Hi_Conga,Open_Hi_Conga,Low_Conga,High_Timbale,Low_Timbale,
	High_Agogo,Low_Agogo,Cabasa,Maracas,Short_Whistle,Long_Whistle,Short_Guiro,Long_Guiro,
	Claves,Hi_Wood_Block,Low_Wood_Block,Mute_Cuica,Open_Cuica,Mute_Triangle,Open_Triangle
};

enum Instrument {
	Acoustic_Grand_Piano = 1, Bright_Acoustic_Piano, Electric_Grand_Piano, Honkytonk_Piano, Electric_Piano_1, Electric_Piano_2, Harpsichord, Clavi, 
	Celesta, 	Glockenspiel, Music_Box, Vibraphone, Marimba, Xylophone, Tubular_Bells, Dulcimer, 
	Drawbar_Organ, Percussive_Organ, Rock_Organ, Church_Organ, Reed_Organ, Accordion, Harmonica, Tango_Accordion, 
	Acoustic_Guitar_nylon, Acoustic_Guitar_steel, Electric_Guitar_jazz, Electric_Guitar_clean, Electric_Guitar_muted, Overdriven_Guitar, Distortion_Guitar, Guitar_harmonics, 
	Acoustic_Bass, Electric_Bass_finger, Electric_Bass_pick, Fretless_Bass, Slap_Bass_1, Slap_Bass_2, Synth_Bass_1, Synth_Bass_2, 
	Violin, Viola, Cello, Contrabass, Tremolo_Strings, Pizzicato_Strings, Orchestral_Harp, Timpani, 
	String_Ensemble_1, String_Ensemble_2, SynthStrings_1, SynthStrings_2, Choir_Aahs, Voice_Oohs, Synth_Voice, Orchestra_Hit, 
	Trumpet, Trombone, Tuba, Muted_Trumpet, French_Horn, Brass_Section, SynthBrass_1, SynthBrass_2, 
	Soprano_Sax, Alto_Sax, Tenor_Sax, Baritone_Sax, Oboe, English_Horn, Bassoon, Clarinet, 
	Piccolo, Flute, Recorder, Pan_Flute, Blown_Bottle, Shakuhachi, Whistle, Ocarina, 
	Lead_1_square, Lead_2_sawtooth, Lead_3_calliope, Lead_4_chiff, Lead_5_charang, Lead_6_voice, Lead_7_fifths, Lead_8_bass_and_lead, 
	Pad_1_new_age, Pad_2_warm, Pad_3_polysynth, Pad_4_choir, Pad_5_bowed, Pad_6_metallic, Pad_7_halo, Pad_8_sweep, 
	FX_1_rain, FX_2_soundtrack, FX_3_crystal, FX_4_atmosphere, FX_5_brightness, FX_6_goblins, FX_7_echoes, FX_8_sci_fi, 
	Sitar, Banjo, Shamisen, Koto, Kalimba, Bag_pipe, Fiddle, Shanai, 
	Tinkle_Bell, Agogo, Steel_Drums, Woodblock, Taiko_Drum, Melodic_Tom, Synth_Drum, Reverse_Cymbal, 
	Guitar_Fret_Noise, Breath_Noise, Seashore, Bird_Tweet, Telephone_Ring, Helicopter, Applause, Gunshot, 
};

enum {
	Piano1 = 1, Piano2, Piano3, Piano4, Piano5, Piano6, Piano7, Piano8,
	Chromatic_Percussion1, Chromatic_Percussion2, Chromatic_Percussion3, Chromatic_Percussion4, Chromatic_Percussion5, Chromatic_Percussion6, Chromatic_Percussion7, Chromatic_Percussion8,
	Organ1, Organ2, Organ3, Organ4, Organ5, Organ6, Organ7, Organ8,
	Guitar1, Guitar2, Guitar3, Guitar4, Guitar5, Guitar6, Guitar7, Guitar8,
	Bass1, Bass2, Bass3, Bass4, Bass5, Bass6, Bass7, Bass8,
	Strings1, Strings2, Strings3, Strings4, Strings5, Strings6, Strings7, Strings8,
	Ensemble1, Ensemble2, Ensemble3, Ensemble4, Ensemble5, Ensemble6, Ensemble7, Ensemble8,
	Brass1, Brass2, Brass3, Brass4, Brass5, Brass6, Brass7, Brass8,
	Reed1, Reed2, Reed3, Reed4, Reed5, Reed6, Reed7, Reed8,
	Pipe1, Pipe2, Pipe3, Pipe4, Pipe5, Pipe6, Pipe7, Pipe8,
	Synth_Lead1, Synth_Lead2, Synth_Lead3, Synth_Lead4, Synth_Lead5, Synth_Lead6, Synth_Lead7, Synth_Lead8,
	Synth_Pad1, Synth_Pad2, Synth_Pad3, Synth_Pad4, Synth_Pad5, Synth_Pad6, Synth_Pad7, Synth_Pad8,
	Synth_Effects1, Synth_Effects2, Synth_Effects3, Synth_Effects4, Synth_Effects5, Synth_Effects6, Synth_Effects7, Synth_Effects8,
	Ethnic1, Ethnic2, Ethnic3, Ethnic4, Ethnic5, Ethnic6, Ethnic7, Ethnic8,
	Percussive1, Percussive2, Percussive3, Percussive4, Percussive5, Percussive6, Percussive7, Percussive8,
	Sound_Effects1, Sound_Effects2, Sound_Effects3, Sound_Effects4, Sound_Effects5, Sound_Effects6, Sound_Effects7, Sound_Effects8,
};

class MIDISoundPlayer {
public:
	enum {
		NOTE_OFF = 128,
		NOTE_ON = 144,
		CONTROL_CHANGE = 176,
		PROGRAM_CHANGE = 192
	};

	enum {
		CONTROL_CHANGE_VOLUME = 7,
		CONTROL_CHANGE_ALL_NOTES_OFF = 123,
	};

	RtMidiOut midiout;

	MIDISoundPlayer(int instrument);

	~MIDISoundPlayer();

	void SetInstrument(unsigned char instrument);

	int GetInstrument();

	std::wstring GetInstrumentName();

	static std::wstring GetInstrumentName(int id);

	void SetVolume(char volume);

	void PlayNote(char note, char on_velocity, char off_velocity, int duration);

	void PlayNotes(const std::vector<char>& notes, char on_velocity, char off_velocity, int duration);

	void StopAllNotes();	

private:
	int selected_instrument = 0;

	int port_count = 0;

	void NoteOn(char note, char velocity);

	void NoteOff(char note, char velocity);
};


























