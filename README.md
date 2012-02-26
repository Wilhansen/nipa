#nipa

Nitro+ &emdash; <http://www.nitroplus.co.jp/>  
Nitro+ CHiRAL &emdash; <http://www.nitrochiral.com/>
 
## Description (from comments)
This is a tool made for Nitro+'s NPA archives which is used by the N2System engine. The engine is used in 6 games total between Nitro+ and Nitro+ CHiRAL. It supports unpacking and repacking even if the format is archived and/or encrypted.
There are 3 versions of the NPA archive that is currently known.
See [npadoc.html](https://github.com/Wilhansen/nipa/blob/master/npadoc.html) for documentation on the NPA format.

## Usage
For the uninitiated, this is a non-interactive command-line program so this is best run from the command line.

* General
    * `-h` &emdash; Help. Displays this information.
* Extraction
    * `-x (file)` &emdash; Extract NPA archive
    * `-g (id)` &emdash; Game ID for encryption (if applicable). Defaults to ChaosHead if no encryption is entered
* Creation
    * `-c (folder) (output file)` &emdash; Create archive
    * `-z` &emdash; Compress files

**DO NOT** compress a CG folder. There is little change in size by doing so and it messes up the engine. This is a problem with the engine and Nitro+ doesn't compress their CG files because of it.

## Examples
`-x nss.npa` &emdash; Extract nss.npa into the folder "nss"  
`-xg nss.npa MuramasaTr` &emdash; Extract nss.npa into the folder "nss" using the Muramasa Trial encryption  
`-c nss nss.npa` &emdash; Create an archive out of the folder "nss"  
`-cz nss nss.npa` &emdash; Create a compressed archive out of the folder "nss"

(Type `-h` in the program to see the list of game IDs to use for decryption/encryption.)

## Supported Games

* Nitro+
    * Chaos;Head
    * Chaos;Head Trial 1
    * Chaos;Head Trial 2
    * FullMetalDaemon MURAMASA
    * FullMetalDaemon MURAMASA Janen Hen (MuramasaAD)
    * FullMetalDaemon MURAMASA Trial
    * Sumaga
    * Demonbane The Best
    * Axanael/Axanael Trial
    * SonicomiTr1
* Nitro+ CHiRAL
    * Lamento -Beyond the Void-
    * sweet pool
    * Zoku Satsuriku no Django

## Credits
Minagi from the [NovelNews.net forum](http://forums.novelnews.net/showthread.php?t=35621) for reverse-engineering the npa format and writing the original code.