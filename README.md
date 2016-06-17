# EPOC2LSL 
Pushes raw EEG data from the Emotiv EPOC to a labstreaminglayer outlet

Based on 
- Emotiv premium libraries v3.3.1
- LabStreamingLayer v1.11
- Visual Studio 2015

## Executable
Download bin.zip from [here](https://github.com/liarosge/EPOC2LSL/releases) to get the executable.

## Compile from scratch
To compile the project you will have to
- Include edk.lib to folders lib\win32 and lib\x64
- Include IEegData.h (located in your emotiv premium libraries folder) to \include\emotiv folder

To run the executable
- Include edk.dll and [liblsl v1.11] (ftp://sccn.ucsd.edu/pub/software/LSL/)  dll files to "bin" folder
