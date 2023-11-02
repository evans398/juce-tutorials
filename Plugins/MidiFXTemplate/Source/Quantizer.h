#pragma once

class Quantizer
{
public:
    // Function to check if a MIDI note number is in the C major scale and return the closest note
    static int getClosestCMajorScaleNote(int midiNoteNumber)
    {
        // MIDI note numbers for the C major scale
        std::vector<int> cMajorScale = {0, 2, 4, 5, 7, 9, 11};  // C, D, E, F, G, A, B
        
        // Calculate the closest note in the C major scale
        int closestNote = midiNoteNumber;
        int minDistance = 128;  // Set an initial large value

        for (int note : cMajorScale) {
            for (int octave = 0; octave <= 10; octave++) {
                int cNote = note + octave * 12;
                int distance = std::abs(midiNoteNumber - cNote);

                if (distance < minDistance) {
                    minDistance = distance;
                    closestNote = cNote;
                }
            }
        }

        return closestNote;
    }

};
