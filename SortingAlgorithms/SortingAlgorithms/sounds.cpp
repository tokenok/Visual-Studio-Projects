#include "sounds.h"

#include <map>
#include <algorithm>

#define X(x) { Instrument::x, L#x },
std::map<unsigned, std::wstring> InstrumentNames = {
	X(Acoustic_Grand_Piano)X(Bright_Acoustic_Piano)X(Electric_Grand_Piano)X(Honkytonk_Piano)X(Electric_Piano_1)X(Electric_Piano_2)X(Harpsichord)X(Clavi)
	X(Celesta)	X(Glockenspiel)X(Music_Box)X(Vibraphone)X(Marimba)X(Xylophone)X(Tubular_Bells)X(Dulcimer)
	X(Drawbar_Organ)X(Percussive_Organ)X(Rock_Organ)X(Church_Organ)X(Reed_Organ)X(Accordion)X(Harmonica)X(Tango_Accordion)
	X(Acoustic_Guitar_nylon)X(Acoustic_Guitar_steel)X(Electric_Guitar_jazz)X(Electric_Guitar_clean)X(Electric_Guitar_muted)X(Overdriven_Guitar)X(Distortion_Guitar)X(Guitar_harmonics)
	X(Acoustic_Bass)X(Electric_Bass_finger)X(Electric_Bass_pick)X(Fretless_Bass)X(Slap_Bass_1)X(Slap_Bass_2)X(Synth_Bass_1)X(Synth_Bass_2)
	X(Violin)X(Viola)X(Cello)X(Contrabass)X(Tremolo_Strings)X(Pizzicato_Strings)X(Orchestral_Harp)X(Timpani)
	X(String_Ensemble_1)X(String_Ensemble_2)X(SynthStrings_1)X(SynthStrings_2)X(Choir_Aahs)X(Voice_Oohs)X(Synth_Voice)X(Orchestra_Hit)
	X(Trumpet)X(Trombone)X(Tuba)X(Muted_Trumpet)X(French_Horn)X(Brass_Section)X(SynthBrass_1)X(SynthBrass_2)
	X(Soprano_Sax)X(Alto_Sax)X(Tenor_Sax)X(Baritone_Sax)X(Oboe)X(English_Horn)X(Bassoon)X(Clarinet)
	X(Piccolo)X(Flute)X(Recorder)X(Pan_Flute)X(Blown_Bottle)X(Shakuhachi)X(Whistle)X(Ocarina)
	X(Lead_1_square)X(Lead_2_sawtooth)X(Lead_3_calliope)X(Lead_4_chiff)X(Lead_5_charang)X(Lead_6_voice)X(Lead_7_fifths)X(Lead_8_bass_and_lead)
	X(Pad_1_new_age)X(Pad_2_warm)X(Pad_3_polysynth)X(Pad_4_choir)X(Pad_5_bowed)X(Pad_6_metallic)X(Pad_7_halo)X(Pad_8_sweep)
	X(FX_1_rain)X(FX_2_soundtrack)X(FX_3_crystal)X(FX_4_atmosphere)X(FX_5_brightness)X(FX_6_goblins)X(FX_7_echoes)X(FX_8_sci_fi)
	X(Sitar)X(Banjo)X(Shamisen)X(Koto)X(Kalimba)X(Bag_pipe)X(Fiddle)X(Shanai)
	X(Tinkle_Bell)X(Agogo)X(Steel_Drums)X(Woodblock)X(Taiko_Drum)X(Melodic_Tom)X(Synth_Drum)X(Reverse_Cymbal)
	X(Guitar_Fret_Noise)X(Breath_Noise)X(Seashore)X(Bird_Tweet)X(Telephone_Ring)X(Helicopter)X(Applause)X(Gunshot)
};
#undef X

MIDISoundPlayer::MIDISoundPlayer(int instrument) {
	if ((port_count = midiout.getPortCount())) {
		if (!midiout.isPortOpen())
			midiout.openPort(0);
	}

	selected_instrument = instrument;
	std::vector<unsigned char> message(3);
	message[0] = PROGRAM_CHANGE;
	message[1] = selected_instrument;
	midiout.sendMessage(&message);
}

MIDISoundPlayer::~MIDISoundPlayer() {
	if (!midiout.isPortOpen()) midiout.closePort();
}

void MIDISoundPlayer::SetInstrument(unsigned char instrument) {
	selected_instrument = instrument;
	std::vector<unsigned char> message(3);
	message[0] = PROGRAM_CHANGE;
	message[1] = instrument;
	midiout.sendMessage(&message);
}

int MIDISoundPlayer::GetInstrument() {
	return selected_instrument;
}

std::wstring MIDISoundPlayer::GetInstrumentName() {
	return GetInstrumentName(selected_instrument);
}

std::wstring MIDISoundPlayer::GetInstrumentName(int id) {
	return InstrumentNames[id];
}

void MIDISoundPlayer::SetVolume(char volume) {
	std::vector<unsigned char> message(3);
	message[0] = CONTROL_CHANGE;
	message[1] = CONTROL_CHANGE_VOLUME;
	message[2] = (unsigned char)volume;
	midiout.sendMessage(&message);
}

void MIDISoundPlayer::PlayNote(char note, char on_velocity, char off_velocity, int duration) {
	std::thread a{ [](MIDISoundPlayer* midi, char note, char on_velocity, char off_velocity, int duration) {
		midi->NoteOn(note, on_velocity);
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
		midi->NoteOff(note, off_velocity);
	}, this, note, on_velocity, off_velocity, duration };
	a.detach();
}

void MIDISoundPlayer::PlayNotes(const std::vector<char>& notes, char on_velocity, char off_velocity, int duration) {
	std::thread a{[](MIDISoundPlayer* midi, std::vector<char> notes, char on_velocity, char off_velocity, int duration) {
		for (auto & note : notes) midi->NoteOn(note, on_velocity);
		std::this_thread::sleep_for(std::chrono::milliseconds(duration));
		for (auto & note : notes) midi->NoteOff(note, off_velocity);
	}, this, std::vector<char>(notes), on_velocity, off_velocity, duration};
	a.detach();
}

void MIDISoundPlayer::StopAllNotes() {
	std::vector<unsigned char> message(3);
	message[0] = CONTROL_CHANGE;
	message[1] = CONTROL_CHANGE_ALL_NOTES_OFF;
	midiout.sendMessage(&message);
}

void MIDISoundPlayer::NoteOn(char note, char velocity) {
	std::vector<unsigned char> message(3);
	message[0] = NOTE_ON;
	message[1] = (unsigned char)note;
	message[2] = (unsigned char)velocity;
	midiout.sendMessage(&message);
}

void MIDISoundPlayer::NoteOff(char note, char velocity) {
	std::vector<unsigned char> message(3);
	message[0] = NOTE_OFF;
	message[1] = (unsigned char)note;
	message[2] = (unsigned char)velocity;
	midiout.sendMessage(&message);
}
































