# Highly portable MPEG Transport Stream library

## Main features:
* No memory allocations: library can perform operation entaerly on stack
* Modularity: some components of the library can be easily excluded

## Components
1. Core:
 * Structures:
   - [x] MpegTsPacket
   - [ ] MpegTsAdaptationField
   - [ ] MpegTsDescriptor
      - [x] MpegTsLanguageDescriptor
      - [x] MpegTsTeletextDescriptor
 * Builders:
   - [ ] MpegTsPATBuilder  (Program association table builder) 
   - [x] MpegTsPMTBuilder  (Program map table builder) 
   - [ ] MpegTsCATBuilder  (Conditional access table builder) 
   - [ ] MpegTsNUTBuilder  (Network information table builder) 
   - [ ] MpegTsTSDTBuilder (Transport stream description table) 
   - [ ] MpegTsPMTBuilder  (Program map table builder) 
2. Output:
   - [ ] JSON5
      - [ ] MpegTsDescriptorDumper
         - [x] MpegTsLanguageDescriptor
      - [x] MpegTsPMTDumper


## Build steps
TODO
## Usage
TODO
