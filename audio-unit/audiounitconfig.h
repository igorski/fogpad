// The specific variant of the Audio Unit app extension.
// The four possible types and their values are:
// Effect (aufx), Generator (augn), Instrument (aumu), and Music Effect (aufm)
#define kAUcomponentType 'aufx'
#define kAUcomponentType1 aufx

// A subtype code (unique ID) for the audio unit, such as gav3.
// This value must be exactly 4 alphanumeric characters
#define kAUcomponentSubType 'rvb2'
#define kAUcomponentSubType1 rvb2

// A manufacturer code for the audio unit, such as Aaud.
// This value must be exactly 4 alphanumeric characters
#define kAUcomponentManufacturer 'IGOR'
#define kAUcomponentManufacturer1 IGOR

// A product name for the audio unit
#define kAUcomponentDescription AUv3WrapperExtension

// The full name of the audio unit.
// This is derived from the manufacturer and description key values
#define kAUcomponentName Igorski: FogPad

// Displayed Tags
#define kAUcomponentTag Effects

// A version number for the Audio Unit app extension (decimal value of hexadecimal representation with zeros between subversions)
// Hexadecimal indexes representing: [0] = main version, [1] = 0 = dot, [2] = sub version, [3] = 0 = dot, [4] = sub-sub version,
// e.g. 1.0.0 == 0x10000 == 65536, 1.2.3 = 0x10203 = 66051
#define kAUcomponentVersion 65539

// Supported number of channels of your audio unit.
// Integer indexes representing: [0] = input count, [1] = output count, [2] = 2nd input count,
// [3]=2nd output count, etc.
// e.g. 1122 == config1: [mono input, mono output], config2: [stereo input, stereo output]
// see channelCapabilities for discussion
#define kSupportedNumChannels 1122

// The preview audio file name.
// To add your own custom audio file (for standalone effects), add an audio file to the project (AUv3WrappermacOS and AUv3WrapperiOS targets) and
// enter the file name here
// #define kAudioFileName "drumLoop"

// The preview audio file format.
// To add your own custom audio file (for standalone effects), add an audio file to the project (AUv3WrappermacOS and AUv3WrapperiOS targets) and
// enter the file format here
// #define kAudioFileFormat "wav"

// componentFlags (leave at 0)
#define kAUcomponentFlags 0

// componentFlagsMask (leave at 0)
#define kAUcomponentFlagsMask 0

// class name for the application delegate
#define kAUapplicationDelegateClassName AppDelegate
