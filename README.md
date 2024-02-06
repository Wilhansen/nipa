#nipa

Nitro+ — <http://www.nitroplus.co.jp/>  
Nitro+ CHiRAL — <http://www.nitrochiral.com/>
 
## Description (from comments)
This is a tool made for Nitro+'s NPA archives which is used by the N2System engine. The engine is used in 6 games total between Nitro+ and Nitro+ CHiRAL. It supports unpacking and repacking even if the format is archived and/or encrypted.
There are 3 versions of the NPA archive that is currently known.
See [npadoc.html](http://www.tsukuru.info/nipa/npadoc.html) (also archived here) for documentation on the NPA format.

## Usage
For the uninitiated, this is a non-interactive command-line program so this is best run from the command line.

* General
    * `-h` — Displays help information and lists Game IDs.
* Extraction
    * `-x (file)` — Extract NPA archive
    * `-g (id)` — Game ID for encryption (if applicable). Defaults to ChaosHead if no encryption is entered
* Creation
    * `-c (folder) (output file)` — Create archive
    * `-z` — Compress files
    * `-cg (folder) (output file) (id)` — Create encrypted archive using game id.

**DO NOT** compress a CG folder. There is little change in size by doing so and it messes up the engine. This is a problem with the engine and Nitro+ doesn't compress their CG files because of it.

## Examples
`-x nss.npa` — Extract nss.npa into the folder "nss"  
`-xg nss.npa MuramasaTr` — Extract nss.npa into the folder "nss" using the Muramasa Trial encryption  
`-c nss nss.npa` — Create an archive out of the folder "nss"  
`-cz nss nss.npa` — Create a compressed archive out of the folder "nss"  
`-czg nss nss.npa Lamento` — Create a compressed archive of the folder "nss" then use Lamento encryption.

## Supported Games
Game IDs in `code` formatting.

* Nitro+
    * Chaos;Head `ChaosHead`
    * Chaos;Head Trial 1 `ChaosHeadTr1`
    * Chaos;Head Trial 2 `ChaosHeadTr2`
    * FullMetalDaemon MURAMASA `Muramasa`
    * FullMetalDaemon MURAMASA Janen Hen `MuramasaAD`
    * FullMetalDaemon MURAMASA Trial `MuramasaTr`
	* FullMetalDaemon MURAMASA Shokuzai hen `MuramasaSS`
    * Guilty Crown Lost Xmas Trailer `LostXTrailer`
    * Guilty Crown Lost Xmas `LostX`
    * Kikokugai `Kikokugai`
    * Kimi to Kajono to Kanojo no Koi `Totono`
    * Sumaga `Sumaga`
	* Sumaga 3% `Sumaga3P`
    * Sumaga Special `SumagaSP`
    * Demonbane The Best ` Demonbane`
    * Axanael/Axanael Trial `Axanael`
	* Sonicomi `Sonicomi`
    * Sonicomi Trial 2 `SonicomiTr2`
    * Zoku Satsuriku no Django `Django`
* Nitro+ CHiRAL
    * Lamento -Beyond the Void- `Lamento`
    * sweet pool `sweetpool`
    * DRAMAtical Murder `DramaticalMurder`
	* DRAMAtical Murder re:connect `DramaticalMurderRC`

## Credits
Minagi from the [NovelNews.net forum](https://web.archive.org/web/20230515162623/http://forums.novelnews.net/showthread.php?t=35621) for reverse-engineering the npa format and writing the original code.
